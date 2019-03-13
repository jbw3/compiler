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

VariableExpression::VariableExpression(const string& name) : name(name)
{
}

void VariableExpression::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& VariableExpression::GetName() const
{
    return name;
}

VariableDefinition::VariableDefinition(const string& name) : name(name)
{
}

void VariableDefinition::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& VariableDefinition::GetName() const
{
    return name;
}

FunctionDefinition::FunctionDefinition(const string& name, SyntaxTreeNode* code) :
    name(name),
    code(code)
{
}

FunctionDefinition::~FunctionDefinition()
{
    for (VariableDefinition* param : parameters)
    {
        delete param;
    }
    delete code;
}

void FunctionDefinition::Accept(SyntaxTreeVisitor* visitor) const
{
    visitor->Visit(this);
}

const string& FunctionDefinition::GetName() const
{
    return name;
}

const SyntaxTreeNode* FunctionDefinition::GetCode() const
{
    return code;
}
} // namespace SyntaxTree
