#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"

using namespace std;

namespace SyntaxTree
{
NumericExpression::NumericExpression(string number) : number(number)
{
}

void NumericExpression::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& NumericExpression::GetNumber() const
{
    return number;
}

BinaryExpression::BinaryExpression(EOperator op, Expression* left, Expression* right) :
    op(op),
    left(left),
    right(right)
{
}

BinaryExpression::~BinaryExpression()
{
    delete left;
    delete right;
}

void BinaryExpression::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

BinaryExpression::EOperator BinaryExpression::GetOperator() const
{
    return op;
}

const Expression* BinaryExpression::GetLeftExpression() const
{
    return left;
}

const Expression* BinaryExpression::GetRightExpression() const
{
    return right;
}

Variable::Variable(const string& name) : name(name)
{
}

void Variable::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& Variable::GetName() const
{
    return name;
}

Assignment::Assignment(Variable* variable, Expression* expression) :
    variable(variable),
    expression(expression)
{
}

Assignment::~Assignment()
{
    delete variable;
    delete expression;
}

void Assignment::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const Variable* Assignment::GetVariable() const
{
    return variable;
}

const Expression* Assignment::GetExpression() const
{
    return expression;
}

Function::Function(const string& name, SyntaxTreeNode* code) : name(name), code(code)
{
}

Function::~Function()
{
    for (Variable* param : parameters)
    {
        delete param;
    }
    delete code;
}

void Function::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& Function::GetName() const
{
    return name;
}

const SyntaxTreeNode* Function::GetCode() const
{
    return code;
}
} // namespace SyntaxTree
