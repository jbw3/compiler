#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#pragma clang diagnostic pop

class LlvmIrGenerator : public SyntaxTreeVisitor
{
public:
    LlvmIrGenerator();

    void Visit(const SyntaxTree::Assignment* assignment) override;

    void Visit(const SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(const SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(const SyntaxTree::Variable* variable) override;

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Value* resultValue;
};

#endif // LLVM_IR_GENERATOR_H_
