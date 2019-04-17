#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "LlvmIrGenerator.h"
#include "SyntaxTree.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "utils.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace std;
using namespace SyntaxTree;
using namespace llvm;

LlvmIrGenerator::LlvmIrGenerator(const Config& config) :
    builder(context),
    module("module", context),
    architecture(config.architecture),
    assemblyType(config.assemblyType),
    resultValue(nullptr)
{
    if (config.outFilename.empty())
    {
        size_t idx = config.inFilename.rfind('.');
        string baseName = config.inFilename.substr(0, idx);

        switch (config.assemblyType)
        {
            case Config::eLlvmIr:
                outFilename = baseName + ".ll";
                break;
            case Config::eMachineText:
                outFilename = baseName + ".s";
                break;
            case Config::eMachineBinary:
                outFilename = baseName + ".o";
                break;
        }
    }
    else
    {
        outFilename = config.outFilename;
    }
}

void LlvmIrGenerator::Visit(SyntaxTree::UnaryExpression* unaryExpression)
{
    unaryExpression->GetSubExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* subExprValue = resultValue;

    switch (unaryExpression->GetOperator())
    {
        case UnaryExpression::eNegative:
            resultValue = builder.CreateNeg(subExprValue, "neg");
            break;
    }
}

void LlvmIrGenerator::Visit(BinaryExpression* binaryExpression)
{
    binaryExpression->GetLeftExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* leftValue = resultValue;

    binaryExpression->GetRightExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* rightValue = resultValue;

    switch (binaryExpression->GetOperator())
    {
        case BinaryExpression::eEqual:
            resultValue = builder.CreateICmpEQ(leftValue, rightValue, "cmpeq");
            break;
        case BinaryExpression::eNotEqual:
            resultValue = builder.CreateICmpNE(leftValue, rightValue, "cmpne");
            break;
        case BinaryExpression::eLessThan:
            resultValue = builder.CreateICmpSLT(leftValue, rightValue, "cmplt");
            break;
        case BinaryExpression::eLessThanOrEqual:
            resultValue = builder.CreateICmpSLE(leftValue, rightValue, "cmple");
            break;
        case BinaryExpression::eGreaterThan:
            resultValue = builder.CreateICmpSGT(leftValue, rightValue, "cmpgt");
            break;
        case BinaryExpression::eGreaterThanOrEqual:
            resultValue = builder.CreateICmpSGE(leftValue, rightValue, "cmpge");
            break;
        case BinaryExpression::eAdd:
            resultValue = builder.CreateAdd(leftValue, rightValue, "add");
            break;
        case BinaryExpression::eSubtract:
            resultValue = builder.CreateSub(leftValue, rightValue, "sub");
            break;
        case BinaryExpression::eMultiply:
            resultValue = builder.CreateMul(leftValue, rightValue, "mul");
            break;
        case BinaryExpression::eDivide:
            resultValue = builder.CreateSDiv(leftValue, rightValue, "div");
            break;
        case BinaryExpression::eModulo:
            resultValue = builder.CreateSRem(leftValue, rightValue, "mod");
            break;
    }
}

void LlvmIrGenerator::Visit(FunctionDefinition* functionDefinition)
{
    const string& funcName = functionDefinition->GetName();
    Function* func = module.getFunction(funcName);
    if (func == nullptr)
    {
        cerr << "Internal error: Function '" << funcName << "' was not declared\n";
        resultValue = nullptr;
        return;
    }

    if (!func->empty())
    {
        cerr << "Cannot redefine function: '" << funcName << "'\n";
        resultValue = nullptr;
        return;
    }

    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(basicBlock);

    currentScope.reset(new Scope());
    size_t idx = 0;
    for (Argument& arg : func->args())
    {
        string paramName = functionDefinition->GetParameters()[idx]->GetName();
        arg.setName(paramName);
        currentScope->AddVariable(&arg);
        ++idx;
    }

    functionDefinition->GetCode()->Accept(this);
    currentScope.reset(nullptr);

    if (resultValue == nullptr)
    {
        return;
    }

    builder.CreateRet(resultValue);

    bool error = verifyFunction(*func, &errs());
    if (error)
    {
        resultValue = nullptr;
        cerr << "Internal error verifying function\n";
        return;
    }
}

void LlvmIrGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    // create function declarations
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        bool ok = CreateFunctionDeclaration(funcDef);
        if (!ok)
        {
            resultValue = nullptr;
            return;
        }
    }

    // generate code for functions
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        funcDef->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
    }
}

void LlvmIrGenerator::Visit(NumericExpression* numericExpression)
{
    int64_t number = 0;
    bool ok = stringToInteger(numericExpression->GetNumber(), number);
    if (ok)
    {
        resultValue = ConstantInt::get(context, APInt(32, number, true));
    }
    else
    {
        resultValue = nullptr;
        cerr << "Invalid numeric literal \"" << numericExpression->GetNumber() << "\"\n";
    }
}

void LlvmIrGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    bool value = false;
    bool ok = stringToBool(boolLiteralExpression->GetValue(), value);
    if (ok)
    {
        resultValue = value ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
    }
    else
    {
        resultValue = nullptr;
        cerr << "Invalid boolean literal \"" << boolLiteralExpression->GetValue() << "\"\n";
    }
}

void LlvmIrGenerator::Visit(VariableExpression* variableExpression)
{
    const string& name = variableExpression->GetName();
    resultValue = currentScope->GetVariable(name);
    if (resultValue == nullptr)
    {
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }
}

void LlvmIrGenerator::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    Function* func = module.getFunction(funcName);
    if (func == nullptr)
    {
        resultValue = nullptr;
        cerr << "Use of undeclared function \"" << funcName << "\"\n";
        return;
    }

    if (functionExpression->GetArguments().size() != func->arg_size())
    {
        resultValue = nullptr;
        cerr << "Unexpected number of function arguments\n";
        return;
    }

    vector<Value*> args;
    for (Expression* expr : functionExpression->GetArguments())
    {
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        args.push_back(resultValue);
    }

    resultValue = builder.CreateCall(func, args, "call");
}

void LlvmIrGenerator::Visit(BranchExpression* branchExpression)
{
    // generate the if condition IR
    branchExpression->GetIfCondition()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* ifConditionValue = resultValue;

    // create the branch basic blocks
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* ifBlock = BasicBlock::Create(context, "if", function);
    BasicBlock* elseBlock = BasicBlock::Create(context, "else");
    BasicBlock* mergeBlock = BasicBlock::Create(context, "merge");

    builder.CreateCondBr(ifConditionValue, ifBlock, elseBlock);

    // generate "if" branch IR
    builder.SetInsertPoint(ifBlock);
    branchExpression->GetIfExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* ifExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "if" block
    ifBlock = builder.GetInsertBlock();

    // generate "else" branch IR
    function->getBasicBlockList().push_back(elseBlock);
    builder.SetInsertPoint(elseBlock);
    branchExpression->GetElseExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* elseExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "else" block
    elseBlock = builder.GetInsertBlock();

    // generate merge block
    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    Type* phiType = GetType(branchExpression->GetType());
    PHINode* phiNode = builder.CreatePHI(phiType, 2, "phi");
    phiNode->addIncoming(ifExprValue, ifBlock);
    phiNode->addIncoming(elseExprValue, elseBlock);

    resultValue = phiNode;
}

bool LlvmIrGenerator::GenerateCode(SyntaxTreeNode* syntaxTree)
{
    // generate LLVM IR from syntax tree
    syntaxTree->Accept(this);
    if (resultValue == nullptr)
    {
        return false;
    }

    if (assemblyType == Config::eLlvmIr)
    {
        error_code ec;
        raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
        if (ec)
        {
            cerr << ec.message();
            return false;
        }

        // print LLVM IR
        module.print(outFile, nullptr);
    }
    else if (assemblyType == Config::eMachineText || assemblyType == Config::eMachineBinary)
    {
        // default target triple to the current machine
        Triple targetTripple(sys::getDefaultTargetTriple());

        // override the architecture if configured
        if (!architecture.empty())
        {
            Triple::ArchType archType = Triple::getArchTypeForLLVMName(architecture);
            targetTripple.setArch(archType);
        }

        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        string errorMsg;
        const Target* target = TargetRegistry::lookupTarget(targetTripple.str(), errorMsg);
        if (target == nullptr)
        {
            cerr << errorMsg;
            return false;
        }

        TargetOptions options;
        auto relocModel = Optional<Reloc::Model>();
        TargetMachine* targetMachine =
            target->createTargetMachine(targetTripple.str(), "generic", "", options, relocModel);

        module.setDataLayout(targetMachine->createDataLayout());
        module.setTargetTriple(targetTripple.str());

        error_code ec;
        raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
        if (ec)
        {
            cerr << ec.message();
            return false;
        }

        legacy::PassManager passManager;
        TargetMachine::CodeGenFileType fileType = (assemblyType == Config::eMachineBinary)
                                                      ? TargetMachine::CGFT_ObjectFile
                                                      : TargetMachine::CGFT_AssemblyFile;
        if (targetMachine->addPassesToEmitFile(passManager, outFile, nullptr, fileType))
        {
            cerr << "Target machine cannot emit a file of this type\n";
            return false;
        }

        passManager.run(module);
    }
    else
    {
        cerr << "Internal error: Unknown assembly type\n";
        return false;
    }

    return true;
}

Type* LlvmIrGenerator::GetType(EType type)
{
    Type* llvmType = nullptr;
    switch (type)
    {
        case EType::eUnknown:
            llvmType = nullptr;
            break;
        case EType::eBool:
            llvmType = Type::getInt1Ty(context);
            break;
        case EType::eInt32:
            llvmType = Type::getInt32Ty(context);
            break;
    }

    return llvmType;
}

bool LlvmIrGenerator::CreateFunctionDeclaration(SyntaxTree::FunctionDefinition* funcDef)
{
    // get the return type
    Type* returnType = GetType(funcDef->GetReturnType());
    if (returnType == nullptr)
    {
        cerr << "Internal error: invalid function return type\n";
        return false;
    }

    // get the parameter types
    vector<Type*> parameters;
    parameters.reserve(funcDef->GetParameters().size());
    for (const VariableDefinition* varDef : funcDef->GetParameters())
    {
        Type* varType = GetType(varDef->GetType());
        parameters.push_back(varType);
    }

    FunctionType* funcType = FunctionType::get(returnType, parameters, false);
    llvm::Function::Create(funcType, Function::ExternalLinkage, funcDef->GetName(), &module);

    return true;
}
