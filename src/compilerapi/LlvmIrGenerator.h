#ifndef LLVM_IR_GENERATOR_H_
#define LLVM_IR_GENERATOR_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "SymbolTable.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <algorithm>
#include <stack>
#include <unordered_map>
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

namespace llvm
{
class TargetMachine;
}
class CompilerContext;
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
    LlvmIrGenerator(CompilerContext& compilerContext, const Config& config, ErrorLogger& logger);

    ~LlvmIrGenerator();

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ForLoop* forLoop) override;

    void Visit(SyntaxTree::LoopControl* loopControl) override;

    void Visit(SyntaxTree::Return* ret) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::StructDefinition* structDefinition) override;

    void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::Modules* modules) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::IdentifierExpression* identifierExpression) override;

    void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::CastExpression* castExpression) override;

    void Visit(SyntaxTree::ImplicitCastExpression* castExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::ConstantDeclaration* constantDeclaration) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

    bool Generate(SyntaxTree::Modules* syntaxTree, llvm::Module*& module);

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

    struct LoopInfo
    {
        llvm::BasicBlock* breakBlock;
        llvm::BasicBlock* continueBlock;
    };

    llvm::TargetMachine* targetMachine;
    unsigned optimizationLevel;
    bool dbgInfo;
    bool boundsCheck;
    CompilerContext& compilerContext;
    ErrorLogger& logger;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::DIBuilder* diBuilder;
    llvm::DIFile** diFiles;
    std::stack<llvm::DIScope*> diScopes;
    llvm::Module* module;
    SymbolTable symbolTable;
    llvm::Function* currentFunction;
    SyntaxTree::FunctionDefinition* currentFunctionDefinition;
    llvm::DIFile* currentDiFile;
    llvm::Value* resultValue;
    llvm::StructType* unitType;
    llvm::StructType* strStructType;
    unsigned int globalStringCounter;
    std::unordered_map<std::vector<char>, llvm::Constant*> strings;
    llvm::Type* boolType;
    std::unordered_map<std::string, llvm::Type*> types;
    std::unordered_map<std::string, llvm::DICompositeType*> diStructTypes;
    std::stack<LoopInfo> loops;

    llvm::Value* GenerateIntSubscriptIr(const SyntaxTree::BinaryExpression* binaryExpression, llvm::Value* leftValue, llvm::Value* rightValue);

    llvm::Value* GenerateRangeSubscriptIr(const SyntaxTree::BinaryExpression* binaryExpression, llvm::Value* leftValue, llvm::Value* rightValue);

    llvm::Constant* CreateConstantString(const std::string& str);

    llvm::Constant* CreateConstantString(const std::vector<char>& chars);

    llvm::Value* CreateConstantValue(const TypeInfo* type, unsigned constIdx);

    llvm::Type* GetType(const TypeInfo* type);

    llvm::DIType* GetDebugType(const TypeInfo* type);

    bool CreateFunctionDeclaration(const SyntaxTree::FunctionDeclaration* funcDecl);

    llvm::AllocaInst* CreateVariableAlloc(llvm::Function* function, llvm::Type* type, const std::string& paramName);

    llvm::Value* CreateExt(llvm::Value* value, const TypeInfo* valueType, const TypeInfo* dstType);

    llvm::Value* CreateLogicalBranch(SyntaxTree::Expression* conditionExpr, SyntaxTree::Expression* branchExpr, bool isTrueBranch);

    void SetDebugLocation(const Token* token);

    void CreateDebugVariable(const Token* token, const TypeInfo* type, llvm::AllocaInst* alloca);
};

#endif // LLVM_IR_GENERATOR_H_
