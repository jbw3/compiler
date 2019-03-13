#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "LlvmIrGenerator.h"
#include "SyntaxTree.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "utils.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace std;
using namespace SyntaxTree;
using namespace llvm;

LlvmIrGenerator::LlvmIrGenerator() :
    builder(context),
    module("module", context),
    resultValue(nullptr)
{
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
    functionDefinition->GetCode()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    FunctionType* funcType = FunctionType::get(Type::getInt32Ty(context), false);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                                  functionDefinition->GetName(), &module);

    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(basicBlock);
    builder.CreateRet(resultValue);

    bool error = verifyFunction(*func);
    if (error)
    {
        resultValue = nullptr;
        cerr << "Internal error verifying function\n";
        return;
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

void LlvmIrGenerator::Visit(const VariableDefinition* variableDefinition)
{
    // TODO: implement this
    resultValue = nullptr;
}

void LlvmIrGenerator::Visit(const VariableExpression* variableExpression)
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

    module.print(outs(), nullptr);

    return true;
}
