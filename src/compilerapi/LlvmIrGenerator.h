#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Config.h"
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
    LlvmIrGenerator(const Config& config);

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    bool GenerateCode(SyntaxTree::SyntaxTreeNode* syntaxTree);

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::string outFilename;
    Config::EAssemblyType assemblyType;
    std::unique_ptr<Scope> currentScope;
    llvm::Value* resultValue;
};

#endif // LLVM_IR_GENERATOR_H_
