#ifndef SYNTAX_TREE_VISITOR_H_
#define SYNTAX_TREE_VISITOR_H_

namespace SyntaxTree
{
class Assignment;
class BinaryExpression;
class BoolLiteralExpression;
class BlockExpression;
class BranchExpression;
class FunctionDefinition;
class FunctionExpression;
class ModuleDefinition;
class NumericExpression;
class UnaryExpression;
class UnitTypeLiteralExpression;
class VariableExpression;
class WhileLoop;
} // namespace SyntaxTree

class SyntaxTreeVisitor
{
public:
    virtual void Visit(SyntaxTree::UnaryExpression* unaryExpression) = 0;

    virtual void Visit(SyntaxTree::BinaryExpression* binaryExpression) = 0;

    virtual void Visit(SyntaxTree::Assignment* assignment) = 0;

    virtual void Visit(SyntaxTree::WhileLoop* whileLoop) = 0;

    virtual void Visit(SyntaxTree::FunctionDefinition* functionDefinition) = 0;

    virtual void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) = 0;

    virtual void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::VariableExpression* variableExpression) = 0;

    virtual void Visit(SyntaxTree::BlockExpression* blockExpression) = 0;

    virtual void Visit(SyntaxTree::FunctionExpression* functionExpression) = 0;

    virtual void Visit(SyntaxTree::BranchExpression* branchExpression) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
