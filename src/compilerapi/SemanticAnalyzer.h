#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include "SyntaxTreeVisitor.h"

namespace SyntaxTree
{
class SyntaxTreeNode;
}

class SemanticAnalyzer : public SyntaxTreeVisitor
{
public:
    SemanticAnalyzer();

    bool Process(SyntaxTree::SyntaxTreeNode* syntaxTree);

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

private:
    bool isError;
};

#endif // SEMANTIC_ANALYZER_H_
