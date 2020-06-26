#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "SymbolTable.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <algorithm>
#include <stack>
#include <unordered_map>
#pragma clang diagnostic pop

namespace llvm
{
class TargetMachine;
}
class Config;
class ErrorLogger;

namespace std
{
    template<>
    struct hash<vector<char>>
    {
        size_t operator()(vector<char> vec) const
        {
            size_t h = 1'550'029'601; // large prime

            size_t size = vec.size();
            h ^= size * 8'191;
            h *= 31;

            size_t numChars = min(size, sizeof(h));
            for (size_t i = 0; i < numChars; ++i)
            {
                size_t temp = vec[i] << (i * 8);
                h ^= temp;
            }
            h *= 31;

            return h;
        }
    };
}

class LlvmIrGenerator : public SyntaxTreeVisitor
{
public:
    LlvmIrGenerator(const Config& config, ErrorLogger& logger);

    ~LlvmIrGenerator();

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::StructDefinition* structDefinition) override;

    void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

    bool Generate(SyntaxTree::SyntaxTreeNode* syntaxTree, llvm::Module*& module);

private:
    static constexpr size_t STR_STRUCT_ELEMENTS_SIZE = 2;

    // This has to be static because the types will be
    // reference in the generated module after this class
    // has gone out of scope. Probably need to find a
    // better way to do this.
    static llvm::Type* strStructElements[STR_STRUCT_ELEMENTS_SIZE];

    class DebugScope
    {
    public:
        DebugScope(
            bool dbgInfoEnabled,
            std::stack<llvm::DIScope*>& scopes,
            llvm::DIScope* diScope
        );

        ~DebugScope();

    private:
        std::stack<llvm::DIScope*>& diScopes;
        bool dbgInfo;
    };

    llvm::TargetMachine* targetMachine;
    std::string inFilename;
    unsigned optimizationLevel;
    bool dbgInfo;
    ErrorLogger& logger;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::DIBuilder* diBuilder;
    llvm::DIFile* diFile;
    std::stack<llvm::DIScope*> diScopes;
    llvm::Module* module;
    SymbolTable symbolTable;
    llvm::Function* currentFunction;
    llvm::Value* resultValue;
    llvm::StructType* unitType;
    llvm::StructType* strStructType;
    llvm::PointerType* strPointerType;
    unsigned int globalStringCounter;
    std::unordered_map<std::vector<char>, llvm::Constant*> strings;
    llvm::Type* boolType;
    std::unordered_map<std::string, llvm::Type*> types;

    llvm::Type* GetType(const TypeInfo* type);

    llvm::DIType* GetDebugType(const TypeInfo* type);

    bool CreateFunctionDeclaration(const SyntaxTree::FunctionDeclaration* funcDecl);

    llvm::AllocaInst* CreateVariableAlloc(llvm::Function* function, llvm::Type* type, const std::string& paramName);

    const TypeInfo* ExtendType(const TypeInfo* srcType, const TypeInfo* dstType, llvm::Value*& value);

    const TypeInfo* ExtendType(const TypeInfo* leftType, const TypeInfo* rightType, llvm::Value*& leftValue, llvm::Value*& rightValue);

    llvm::Value* CreateExt(llvm::Value* value, const TypeInfo* valueType, const TypeInfo* dstType);

    llvm::Value* CreateLogicalBranch(SyntaxTree::Expression* conditionExpr, SyntaxTree::Expression* branchExpr, bool isTrueBranch);

    void SetDebugLocation(const Token* token);
};

#endif // LLVM_IR_GENERATOR_H_
