#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "LlvmIrGenerator.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "utils.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace std;
using namespace SyntaxTree;
using llvm::APInt;
using llvm::BasicBlock;
using llvm::ConstantInt;
using llvm::FunctionType;
using llvm::IRBuilder;
using llvm::Module;
using llvm::outs;
using llvm::Type;
using llvm::Value;

LlvmIrGenerator::LlvmIrGenerator() : builder(context), resultValue(nullptr)
{
}

void LlvmIrGenerator::Visit(const Assignment* assignment)
{
    assignment->GetExpression()->Accept(this);

    // TODO: set the variable
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

void LlvmIrGenerator::Visit(const Function* function)
{
    // TODO: implement this
    resultValue = nullptr;
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

void LlvmIrGenerator::Visit(const Variable* variable)
{
    // TODO: implement this
    resultValue = nullptr;
}

bool LlvmIrGenerator::GenerateCode(const SyntaxTreeNode* syntaxTree)
{
    syntaxTree->Accept(this);
    if (resultValue == nullptr)
    {
        return false;
    }

    Module module("module", context);

    FunctionType* mainFuncType = FunctionType::get(Type::getInt32Ty(context), false);
    llvm::Function* mainFunc =
        llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", &module);

    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(basicBlock);
    builder.CreateRet(resultValue);

    bool error = verifyFunction(*mainFunc);
    if (error)
    {
        cerr << "Internal error verifying function\n";
        return false;
    }

    module.print(outs(), nullptr);

    return true;
}
