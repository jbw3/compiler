#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include <map>

class SemanticAnalyzer : public SyntaxTreeVisitor
{
public:
    SemanticAnalyzer();

    bool Process(SyntaxTree::SyntaxTreeNode* syntaxTree);

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::Assignment* assignment) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

private:
    bool isError;
    std::map<std::string, SyntaxTree::FunctionDefinition*> functions;
    std::map<std::string, SyntaxTree::VariableDefinition*> variables;

    bool CheckUnaryOperatorType(SyntaxTree::UnaryExpression::EOperator op, const TypeInfo* subExprType);

    bool CheckBinaryOperatorTypes(SyntaxTree::BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType);

    const TypeInfo* GetBinaryOperatorResultType(SyntaxTree::BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType);
};

#endif // SEMANTIC_ANALYZER_H_
