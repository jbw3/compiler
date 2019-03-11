#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <string>
#include <vector>

class SyntaxTreeVisitor;

namespace SyntaxTree
{
class SyntaxTreeNode
{
public:
    virtual ~SyntaxTreeNode() = default;

    virtual void Accept(SyntaxTreeVisitor* visitor) const = 0;
};

class Expression : public SyntaxTreeNode
{
public:
    virtual ~Expression() = default;
};

class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetNumber() const;

private:
    std::string number;
};

class BinaryExpression : public Expression
{
public:
    enum EOperator
    {
        eAdd,
        eSubtract,
    };

    BinaryExpression(EOperator op, Expression* left, Expression* right);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    EOperator GetOperator() const;

    const Expression* GetLeftExpression() const;

    const Expression* GetRightExpression() const;

private:
    EOperator op;
    Expression* left;
    Expression* right;
};

class Variable : public SyntaxTreeNode
{
public:
    Variable(const std::string& name);

    virtual ~Variable() = default;

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetName() const;

private:
    std::string name;
};

class Assignment : public SyntaxTreeNode
{
public:
    Assignment(Variable* variable, Expression* expression);

    virtual ~Assignment();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const Variable* GetVariable() const;

    const Expression* GetExpression() const;

private:
    Variable* variable;
    Expression* expression;
};

class Function : public SyntaxTreeNode
{
public:
    Function(const std::string& name, SyntaxTreeNode* code);

    virtual ~Function();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetName() const;

    const SyntaxTreeNode* GetCode() const;

private:
    std::string name;
    std::vector<Variable*> parameters;
    SyntaxTreeNode* code;
};
} // namespace SyntaxTree

#endif // SYNTAX_TREE_H_
