#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Config.h"
#include "Scope.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#pragma clang diagnostic pop

class LlvmIrGenerator : public SyntaxTreeVisitor
{
public:
    LlvmIrGenerator(const Config& config);

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    bool GenerateCode(SyntaxTree::SyntaxTreeNode* syntaxTree);

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;
    std::string outFilename;
    std::string architecture;
    Config::EAssemblyType assemblyType;
    std::unique_ptr<Scope> currentScope;
    llvm::Value* resultValue;
    std::map<std::string, SyntaxTree::FunctionDefinition*> functions;

    llvm::Type* GetType(const TypeInfo* type);

    bool CreateFunctionDeclaration(SyntaxTree::FunctionDefinition* funcDef);

    void ExtendType(const TypeInfo* srcType, const TypeInfo* dstType, llvm::Value*& value);

    void ExtendType(const TypeInfo* leftType, const TypeInfo* rightType, llvm::Value*& leftValue, llvm::Value*& rightValue);

    llvm::Value* CreateBranch(SyntaxTree::Expression* conditionExpr, SyntaxTree::Expression* trueExpr, SyntaxTree::Expression* falseExpr,
                              const char* trueName, const char* falseName, const char* mergeName, const char* phiName);

    llvm::Value* CreateLogicalAnd(SyntaxTree::Expression* leftExpr, SyntaxTree::Expression* rightExpr);

    llvm::Value* CreateLogicalOr(SyntaxTree::Expression* leftExpr, SyntaxTree::Expression* rightExpr);
};

#endif // LLVM_IR_GENERATOR_H_
