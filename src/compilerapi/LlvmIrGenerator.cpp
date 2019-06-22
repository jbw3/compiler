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
        case UnaryExpression::eComplement:
            resultValue = builder.CreateNot(subExprValue, "not");
            break;
    }
}

void LlvmIrGenerator::Visit(BinaryExpression* binaryExpression)
{
    BinaryExpression::EOperator op = binaryExpression->GetOperator();
    Expression* leftExpr = binaryExpression->GetLeftExpression();
    Expression* rightExpr = binaryExpression->GetRightExpression();

    if (op == BinaryExpression::eLogicalAnd)
    {
        resultValue = CreateLogicalAnd(leftExpr, rightExpr);
    }
    else if (op == BinaryExpression::eLogicalOr)
    {
        resultValue = CreateLogicalOr(leftExpr, rightExpr);
    }
    else
    {
        leftExpr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
        Value* leftValue = resultValue;

        rightExpr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
        Value* rightValue = resultValue;

        // check if values need to be sign extended
        const TypeInfo* leftType = leftExpr->GetType();
        const TypeInfo* rightType = rightExpr->GetType();
        ExtendType(leftType, rightType, leftValue, rightValue);

        switch (op)
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
            case BinaryExpression::eShiftLeft:
                resultValue = builder.CreateShl(leftValue, rightValue, "shl");
                break;
            case BinaryExpression::eShiftRightArithmetic:
                resultValue = builder.CreateAShr(leftValue, rightValue, "ashr");
                break;
            case BinaryExpression::eBitwiseAnd:
                resultValue = builder.CreateAnd(leftValue, rightValue, "bitand");
                break;
            case BinaryExpression::eBitwiseXor:
                resultValue = builder.CreateXor(leftValue, rightValue, "bitxor");
                break;
            case BinaryExpression::eBitwiseOr:
                resultValue = builder.CreateOr(leftValue, rightValue, "bitor");
                break;
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
                cerr << "Internal error: logical AND and logical OR operators should have been handled before here\n";
                resultValue = nullptr;
                break;
        }
    }
}

void LlvmIrGenerator::Visit(SyntaxTree::Assignment* assignment)
{
    Expression* expression = assignment->GetExpression();
    expression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    const string& name = assignment->GetVariableName();
    AllocaInst* alloca = currentScope->GetValue(name);
    if (alloca == nullptr)
    {
        resultValue = nullptr;
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }

    // sign extend expression value if needed
    const TypeInfo* expressionType = expression->GetType();
    const TypeInfo* varType = currentScope->GetVariable(name)->GetType();
    ExtendType(expressionType, varType, resultValue);

    resultValue = builder.CreateStore(resultValue, alloca);
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
        VariableDefinition* param = functionDefinition->GetParameters()[idx];
        const string& paramName = param->GetName();
        arg.setName(paramName);
        AllocaInst* alloca = builder.CreateAlloca(arg.getType(), nullptr, paramName);
        builder.CreateStore(&arg, alloca);
        currentScope->AddVariable(paramName, param, alloca);

        ++idx;
    }

    // process statements
    for (SyntaxTreeNode* statement : functionDefinition->GetStatements())
    {
        statement->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
    }

    // process return expression
    Expression* returnExpression = functionDefinition->GetReturnExpression();
    returnExpression->Accept(this);
    currentScope.reset(nullptr);

    if (resultValue == nullptr)
    {
        return;
    }

    // sign extend return value if needed
    const TypeInfo* returnExpressionType = returnExpression->GetType();
    const TypeInfo* returnType = functionDefinition->GetReturnType();
    ExtendType(returnExpressionType, returnType, resultValue);

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
    // create function declarations and build function look-up table
    functions.clear();
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        functions.insert({funcDef->GetName(), funcDef});

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
        unsigned int numBits = numericExpression->GetType()->NumBits;
        resultValue = ConstantInt::get(context, APInt(numBits, number, true));
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
    AllocaInst* alloca = currentScope->GetValue(name);
    if (alloca == nullptr)
    {
        resultValue = nullptr;
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }

    resultValue = builder.CreateLoad(alloca, name);
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

    FunctionDefinition* funcDef = functions.find(funcName)->second;
    vector<Expression*> argExpressions = functionExpression->GetArguments();
    vector<Value*> args;
    for (size_t i = 0; i < argExpressions.size(); ++i)
    {
        Expression* expr = argExpressions[i];
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        // sign extend arg value if needed
        const TypeInfo* argType = expr->GetType();
        const TypeInfo* paramType = funcDef->GetParameters()[i]->GetType();
        ExtendType(argType, paramType, resultValue);

        args.push_back(resultValue);
    }

    resultValue = builder.CreateCall(func, args, "call");
}

void LlvmIrGenerator::Visit(BranchExpression* branchExpression)
{
    resultValue = CreateBranch(
        branchExpression->GetIfCondition(),
        branchExpression->GetIfExpression(),
        branchExpression->GetElseExpression(),
        "if", "else", "merge", "phi"
    );
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

Type* LlvmIrGenerator::GetType(const TypeInfo* type)
{
    Type* llvmType = nullptr;
    if (type == TypeInfo::BoolType)
    {
        llvmType = Type::getInt1Ty(context);
    }
    else if (type->IsInt)
    {
        llvmType = Type::getIntNTy(context, type->NumBits);
    }
    else
    {
        llvmType = nullptr;
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

const TypeInfo* LlvmIrGenerator::ExtendType(const TypeInfo* srcType, const TypeInfo* dstType, Value*& value)
{
    const TypeInfo* resultType = nullptr;

    // sign extend value if needed
    if (srcType->IsInt && dstType->IsInt && srcType->NumBits < dstType->NumBits)
    {
        value = builder.CreateSExt(value, GetType(dstType), "signext");
        resultType = dstType;
    }
    else
    {
        resultType = srcType;
    }

    return resultType;
}

const TypeInfo* LlvmIrGenerator::ExtendType(const TypeInfo* leftType, const TypeInfo* rightType, Value*& leftValue, Value*& rightValue)
{
    const TypeInfo* resultType = nullptr;

    if (leftType->IsInt && rightType->IsInt && leftType->NumBits != rightType->NumBits)
    {
        if (leftType->NumBits < rightType->NumBits)
        {
            leftValue = builder.CreateSExt(leftValue, rightValue->getType(), "signext");
            resultType = rightType;
        }
        else
        {
            rightValue = builder.CreateSExt(rightValue, leftValue->getType(), "signext");
            resultType = leftType;
        }
    }

    return resultType;
}

Value* LlvmIrGenerator::CreateBranch(Expression* conditionExpr, Expression* trueExpr, Expression* falseExpr,
                                     const char* trueName, const char* falseName, const char* mergeName, const char* phiName)
{
    // generate the condition IR
    conditionExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }
    Value* conditionValue = resultValue;

    // create the branch basic blocks
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* trueBlock = BasicBlock::Create(context, trueName, function);
    BasicBlock* falseBlock = BasicBlock::Create(context, falseName);
    BasicBlock* mergeBlock = BasicBlock::Create(context, mergeName);

    builder.CreateCondBr(conditionValue, trueBlock, falseBlock);

    // generate "true" block IR
    builder.SetInsertPoint(trueBlock);
    trueExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }

    // extend type if necessary
    const TypeInfo* trueType = ExtendType(trueExpr->GetType(), falseExpr->GetType(), resultValue);

    Value* ifExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "true" block
    trueBlock = builder.GetInsertBlock();

    // generate "false" block IR
    function->getBasicBlockList().push_back(falseBlock);
    builder.SetInsertPoint(falseBlock);
    falseExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }

    // extend type if necessary
    const TypeInfo* falseType = ExtendType(falseExpr->GetType(), trueExpr->GetType(), resultValue);

    Value* elseExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "false" block
    falseBlock = builder.GetInsertBlock();

    // generate merge block IR
    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    // check if the "true" and "false" expressions have the same type
    if (trueType != falseType)
    {
        cerr << "Internal Error: Branch true and false blocks must have the same type\n";
        return nullptr;
    }

    Type* phiType = GetType(trueType);
    PHINode* phiNode = builder.CreatePHI(phiType, 2, phiName);
    phiNode->addIncoming(ifExprValue, trueBlock);
    phiNode->addIncoming(elseExprValue, falseBlock);

    return phiNode;
}

Value* LlvmIrGenerator::CreateLogicalAnd(Expression* leftExpr, Expression* rightExpr)
{
    BoolLiteralExpression* falseExpr = BoolLiteralExpression::CreateFalseExpression();
    Value* value = CreateBranch(leftExpr, rightExpr, falseExpr, "andtrue", "andfalse", "andmerge", "andphi");
    delete falseExpr;

    return value;
}

Value* LlvmIrGenerator::CreateLogicalOr(Expression* leftExpr, Expression* rightExpr)
{
    BoolLiteralExpression* trueExpr = BoolLiteralExpression::CreateTrueExpression();
    Value* value = CreateBranch(leftExpr, trueExpr, rightExpr, "ortrue", "orfalse", "ormerge", "orphi");
    delete trueExpr;

    return value;
}
