#ifndef SYNTAX_TREE_VISITOR_H_
#define SYNTAX_TREE_VISITOR_H_

namespace SyntaxTree
{
class BinaryExpression;
class FunctionDefinition;
class ModuleDefinition;
class NumericExpression;
class VariableExpression;
} // namespace SyntaxTree

class SyntaxTreeVisitor
{
public:
    virtual void Visit(const SyntaxTree::BinaryExpression* binaryExpression) = 0;

    virtual void Visit(const SyntaxTree::FunctionDefinition* functionDefinition) = 0;

    virtual void Visit(const SyntaxTree::ModuleDefinition* moduleDefinition) = 0;

    virtual void Visit(const SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(const SyntaxTree::VariableExpression* variableExpression) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
