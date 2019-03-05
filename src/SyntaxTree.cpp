#include "SyntaxTree.h"

using namespace std;

namespace SyntaxTree
{
BinaryExpression::BinaryExpression(Expression* left, Expression* right) : left(left), right(right)
{
}

BinaryExpression::~BinaryExpression()
{
    delete left;
    delete right;
}

Variable::Variable(string name) : name(name)
{
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
} // namespace SyntaxTree
