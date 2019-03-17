#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Scope.h"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#pragma clang diagnostic pop

namespace SyntaxTree
{
class SyntaxTreeNode;
} // namespace SyntaxTree

class LlvmIrGenerator : public SyntaxTreeVisitor
{
public:
    LlvmIrGenerator();

    void Visit(const SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(const SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(const SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(const SyntaxTree::VariableDefinition* variableDefinition) override;

    void Visit(const SyntaxTree::VariableExpression* variableExpression) override;

    bool GenerateCode(const SyntaxTree::SyntaxTreeNode* syntaxTree);

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::unique_ptr<Scope> currentScope;
    llvm::Value* resultValue;
};

#endif // LLVM_IR_GENERATOR_H_
