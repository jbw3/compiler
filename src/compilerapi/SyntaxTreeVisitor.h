#ifndef SYNTAX_TREE_VISITOR_H_
#define SYNTAX_TREE_VISITOR_H_

namespace SyntaxTree
{
class BinaryExpression;
class BoolLiteralExpression;
class FunctionDefinition;
class FunctionExpression;
class ModuleDefinition;
class NumericExpression;
class VariableExpression;
} // namespace SyntaxTree

class SyntaxTreeVisitor
{
public:
    virtual void Visit(SyntaxTree::BinaryExpression* binaryExpression) = 0;

    virtual void Visit(SyntaxTree::FunctionDefinition* functionDefinition) = 0;

    virtual void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) = 0;

    virtual void Visit(SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::VariableExpression* variableExpression) = 0;

    virtual void Visit(SyntaxTree::FunctionExpression* functionExpression) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
