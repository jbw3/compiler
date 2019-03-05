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

string NumericExpression::GetNumber() const
{
    return number;
}

BinaryExpression::BinaryExpression(Expression* left, Expression* right) : left(left), right(right)
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

const Expression* BinaryExpression::GetLeftExpression() const
{
    return left;
}

const Expression* BinaryExpression::GetRightExpression() const
{
    return right;
}

Variable::Variable(string name) : name(name)
{
}

void Variable::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

string Variable::GetName() const
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
} // namespace SyntaxTree
