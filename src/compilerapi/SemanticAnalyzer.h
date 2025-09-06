#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include "SymbolTable.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>

class CompilerContext;
class ErrorLogger;

class StartEndTokenFinder : public SyntaxTreeVisitor
{
public:
    const Token* start = nullptr;
    const Token* end = nullptr;

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ForLoop* forLoop) override;

    void Visit(SyntaxTree::LoopControl* loopControl) override;

    void Visit(SyntaxTree::Return* ret) override;

    void Visit(SyntaxTree::FunctionTypeExpression* functionTypeExpression) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::StructDefinition* structDefinition) override;

    void Visit(SyntaxTree::StructDefinitionExpression* structDefinitionExpression) override;

    void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::Modules* modules) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::FloatLiteralExpression* floatLiteralExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::IdentifierExpression* identifierExpression) override;

    void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::CastExpression* castExpression) override;

    void Visit(SyntaxTree::ImplicitCastExpression* castExpression) override;

    void Visit(SyntaxTree::FunctionCallExpression* functionCallExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::ConstantDeclaration* constantDeclaration) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

private:
    void UpdateStart(const Token* newStart);

    void UpdateEnd(const Token* newEnd);
};

class SemanticAnalyzer : public SyntaxTreeVisitor
{
public:
    SemanticAnalyzer(CompilerContext& compilerContext);

    bool Process(SyntaxTree::SyntaxTreeNode* syntaxTree);

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ForLoop* forLoop) override;

    void Visit(SyntaxTree::LoopControl* loopControl) override;

    void Visit(SyntaxTree::Return* ret) override;

    void Visit(SyntaxTree::FunctionTypeExpression* functionTypeExpression) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::StructDefinition* structDefinition) override;

    void Visit(SyntaxTree::StructDefinitionExpression* structDefinitionExpression) override;

    void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::Modules* modules) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::FloatLiteralExpression* floatLiteralExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::IdentifierExpression* identifierExpression) override;

    void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::CastExpression* castExpression) override;

    void Visit(SyntaxTree::ImplicitCastExpression* castExpression) override;

    void Visit(SyntaxTree::FunctionCallExpression* functionCallExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::ConstantDeclaration* constantDeclaration) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

private:
    ErrorLogger& logger;
    unsigned loopLevel;
    bool isError;
    bool isConstDecl;
    std::unordered_map<ROString, TypeInfo*> partialStructTypes;
    CompilerContext& compilerContext;
    SymbolTable symbolTable;
    const SyntaxTree::FunctionDefinition* currentFunction;

    std::unordered_map<ROString, SyntaxTree::ConstantDeclaration*> unresolvedConsts;
    std::unordered_set<ROString> processingConsts;
    std::unordered_map<unsigned, TypeInfo*> incompleteStructTypes;
    std::vector<SyntaxTree::StructInitializationExpression*> incompleteStructInits;
    std::stack<bool> needsStructImplStack;
    std::vector<SyntaxTree::StructDefinitionExpression*> incompleteStructExpressions;
    std::vector<SyntaxTree::ConstantDeclaration*> orderedGlobalConsts;
    std::vector<const Token*> constIdentifierStack;

    void LogExistingIdentifierError(ROString name, const Token* token, const Token* existingToken = nullptr);

    void ProcessConstantDeclarations(std::vector<SyntaxTree::ConstantDeclarations*>& constantDeclarations);

    bool SortTypeDefinitions(SyntaxTree::Modules* modules);

    bool ResolveDependencies(
        SyntaxTree::StructDefinition* structDef,
        const std::unordered_map<ROString, SyntaxTree::StructDefinition*>& nameMap,
        std::vector<SyntaxTree::StructDefinition*>& ordered,
        std::unordered_set<ROString>& resolved,
        std::unordered_set<ROString>& dependents);

    const TypeInfo* TypeExpressionToType(SyntaxTree::Expression* typeExpression);

    bool HaveCompatibleSigns(const TypeInfo* leftType, const TypeInfo* rightType);

    bool HaveCompatibleAssignmentSizes(const TypeInfo* assignType, const TypeInfo* exprType);

    bool AreCompatibleRanges(const TypeInfo* type1, const TypeInfo* type2, const TypeInfo*& outType);

    bool AreCompatibleAssignmentTypes(const TypeInfo* assignType, const TypeInfo* exprType, bool& needsCast);

    const NumericLiteralType* GetBiggestNumLitSizeType(const NumericLiteralType* type1, const NumericLiteralType* type2);

    const TypeInfo* GetBiggestSizeType(const NumericLiteralType* type1, const TypeInfo* type2, unsigned& type1Size, unsigned& type2Size);

    const TypeInfo* GetBiggestSizeType(const TypeInfo* type1, const TypeInfo* type2);

    void FixNumericLiteralExpression(SyntaxTree::Expression* expr, const TypeInfo* resultType);

    bool CheckBinaryOperatorTypes(SyntaxTree::BinaryExpression* binExpr);

    const TypeInfo* GetBinaryOperatorResultType(SyntaxTree::BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType);

    bool SetFunctionDeclarationTypes(SyntaxTree::FunctionDeclaration* functionDeclaration);

    const TypeInfo* InferType(const TypeInfo* inferType, const Token* errorToken);

    const TypeInfo* GetVariableType(SyntaxTree::Expression* typeExpression, const TypeInfo* inferType, const Token* errorToken);

    bool CheckReturnType(const SyntaxTree::FunctionDeclaration* funcDecl, SyntaxTree::Expression* expression, const Token* errorToken, SyntaxTree::Expression*& resultExpression);

    SyntaxTree::Expression* ImplicitCast(SyntaxTree::Expression* expression, const TypeInfo* type);
};

#endif // SEMANTIC_ANALYZER_H_
