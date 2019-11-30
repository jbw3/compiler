#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "SymbolTable.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#pragma clang diagnostic pop

namespace llvm
{
class TargetMachine;
}
class Config;

class LlvmIrGenerator : public SyntaxTreeVisitor
{
public:
    LlvmIrGenerator(const Config& config);

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

    bool Generate(SyntaxTree::SyntaxTreeNode* syntaxTree, llvm::Module*& module);

private:
    llvm::TargetMachine* targetMachine;
    std::string inFilename;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module* module;
    SymbolTable symbolTable;
    llvm::Function* currentFunction;
    llvm::Value* resultValue;
    llvm::StructType* unitType;
    std::map<std::string, const SyntaxTree::FunctionDeclaration*> functions;

    llvm::Type* GetType(const TypeInfo* type);

    bool CreateFunctionDeclaration(const SyntaxTree::FunctionDeclaration* funcDecl);

    llvm::AllocaInst* CreateVariableAlloc(llvm::Function* function, llvm::Type* type, const std::string& paramName);

    const TypeInfo* ExtendType(const TypeInfo* srcType, const TypeInfo* dstType, llvm::Value*& value);

    const TypeInfo* ExtendType(const TypeInfo* leftType, const TypeInfo* rightType, llvm::Value*& leftValue, llvm::Value*& rightValue);

    llvm::Value* CreateExt(llvm::Value* value, const TypeInfo* dstType);

    llvm::Value* CreateBranch(SyntaxTree::Expression* conditionExpr, SyntaxTree::Expression* trueExpr, SyntaxTree::Expression* falseExpr,
                              const char* trueName, const char* falseName, const char* mergeName, const char* phiName);

    llvm::Value* CreateLogicalAnd(SyntaxTree::Expression* leftExpr, SyntaxTree::Expression* rightExpr);

    llvm::Value* CreateLogicalOr(SyntaxTree::Expression* leftExpr, SyntaxTree::Expression* rightExpr);
};

#endif // LLVM_IR_GENERATOR_H_
