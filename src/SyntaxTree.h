#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <string>

namespace SyntaxTree
{
class SyntaxTreeNode
{
};

class Expression : public SyntaxTreeNode
{
};

class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

private:
    std::string number;
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(Expression* left, Expression* right);

    virtual ~BinaryExpression();

private:
    Expression* left;
    Expression* right;
};

class Variable : public SyntaxTreeNode
{
public:
    Variable(std::string name);

    std::string GetName() const;

private:
    std::string name;
};

class Assignment : public SyntaxTreeNode
{
public:
    Assignment(Variable* variable, Expression* expression);

    virtual ~Assignment();

private:
    Variable* variable;
    Expression* expression;
};
} // namespace SyntaxTree

#endif // SYNTAX_TREE_H_
