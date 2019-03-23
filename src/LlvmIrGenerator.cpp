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

LlvmIrGenerator::LlvmIrGenerator(const string& outFile) :
    builder(context),
    module("module", context),
    resultValue(nullptr)
{
    if (outFile.empty())
    {
        outFilename = "output.o";
    }
    else
    {
        outFilename = outFile;
    }
}

void LlvmIrGenerator::Visit(const BinaryExpression* binaryExpression)
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
        case BinaryExpression::eAdd:
            resultValue = builder.CreateAdd(leftValue, rightValue, "add");
            break;
        case BinaryExpression::eSubtract:
            resultValue = builder.CreateSub(leftValue, rightValue, "sub");
            break;
        default:
            resultValue = nullptr;
            break;
    }
}

void LlvmIrGenerator::Visit(const FunctionDefinition* functionDefinition)
{
    vector<Type*> parameters(functionDefinition->GetParameters().size(), Type::getInt32Ty(context));
    FunctionType* funcType = FunctionType::get(Type::getInt32Ty(context), parameters, false);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                                  functionDefinition->GetName(), &module);

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

void LlvmIrGenerator::Visit(const ModuleDefinition* moduleDefinition)
{
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        funcDef->Accept(this);
        if (resultValue == nullptr)
        {
            break;
        }
    }
}

void LlvmIrGenerator::Visit(const NumericExpression* numericExpression)
{
    resultValue = nullptr;

    int64_t number = 0;
    bool ok = stringToInteger(numericExpression->GetNumber(), number);
    if (ok)
    {
        resultValue = ConstantInt::get(context, APInt(32, number, true));
    }
}

void LlvmIrGenerator::Visit(const VariableExpression* variableExpression)
{
    const string& name = variableExpression->GetName();
    resultValue = currentScope->GetVariable(name);
    if (resultValue == nullptr)
    {
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }
}

bool LlvmIrGenerator::GenerateCode(const SyntaxTreeNode* syntaxTree)
{
    syntaxTree->Accept(this);
    if (resultValue == nullptr)
    {
        return false;
    }

    string targetTripple = sys::getDefaultTargetTriple();
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    string errorMsg;
    const Target* target = TargetRegistry::lookupTarget(targetTripple, errorMsg);
    if (target == nullptr)
    {
        cerr << errorMsg;
        return false;
    }

    TargetOptions options;
    auto relocModel = Optional<Reloc::Model>();
    TargetMachine* targetMachine =
        target->createTargetMachine(targetTripple, "generic", "", options, relocModel);

    module.setDataLayout(targetMachine->createDataLayout());
    module.setTargetTriple(targetTripple);

    error_code ec;
    raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
    if (ec)
    {
        cerr << ec.message();
        return false;
    }

    legacy::PassManager passManager;
    TargetMachine::CodeGenFileType fileType = TargetMachine::CGFT_ObjectFile;
    if (targetMachine->addPassesToEmitFile(passManager, outFile, nullptr, fileType))
    {
        cerr << "Target machine cannot emit a file of this type\n";
        return false;
    }

    passManager.run(module);

    // print LLVM IR
    module.print(outs(), nullptr);

    return true;
}
