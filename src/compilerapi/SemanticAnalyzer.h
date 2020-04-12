#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include "ErrorLogger.h"
#include "SymbolTable.h"
#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include <map>
#include <unordered_map>
#include <unordered_set>

class SemanticAnalyzer : public SyntaxTreeVisitor
{
public:
    SemanticAnalyzer(ErrorLogger& logger);

    bool Process(SyntaxTree::SyntaxTreeNode* syntaxTree);

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::TypeDefinition* typeDefinition) override;

    void Visit(SyntaxTree::TypeInitializationExpression* typeInitializationExpression) override;

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

private:
    ErrorLogger& logger;
    bool isError;
    std::map<std::string, const SyntaxTree::FunctionDeclaration*> functions;
    SymbolTable symbolTable;

    bool SortTypeDefinitions(SyntaxTree::ModuleDefinition* moduleDefinition);

    bool ResolveDependencies(
        SyntaxTree::TypeDefinition* typeDef,
        const std::unordered_map<std::string, SyntaxTree::TypeDefinition*>& nameMap,
        std::vector<SyntaxTree::TypeDefinition*>& ordered,
        std::unordered_set<std::string>& resolved,
        std::unordered_set<std::string>& dependents);

    bool CheckUnaryOperatorType(SyntaxTree::UnaryExpression::EOperator op, const TypeInfo* subExprType);

    bool CheckBinaryOperatorTypes(SyntaxTree::BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType);

    const TypeInfo* GetBinaryOperatorResultType(SyntaxTree::BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType);

    bool SetVariableDeclarationType(SyntaxTree::VariableDeclaration* variableDeclaration);

    bool SetFunctionDeclarationTypes(SyntaxTree::FunctionDeclaration* functionDeclaration);
};

#endif // SEMANTIC_ANALYZER_H_
