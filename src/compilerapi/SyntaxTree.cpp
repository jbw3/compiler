#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"

using namespace std;

namespace SyntaxTree
{
Expression::Expression() :
    type(eUnknown)
{
}

Expression::EType Expression::GetType() const
{
    return type;
}

void Expression::SetType(EType newType)
{
    type = newType;
}

NumericExpression::NumericExpression(string number) : number(number)
{
}

void NumericExpression::Accept(SyntaxTreeVisitor* visitor)
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

void BinaryExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

BinaryExpression::EOperator BinaryExpression::GetOperator() const
{
    return op;
}

Expression* BinaryExpression::GetLeftExpression() const
{
    return left;
}

Expression* BinaryExpression::GetRightExpression() const
{
    return right;
}

VariableExpression::VariableExpression(const string& name) : name(name)
{
}

void VariableExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& VariableExpression::GetName() const
{
    return name;
}

FunctionExpression::FunctionExpression(const string& name, const vector<Expression*>& arguments) :
    name(name),
    arguments(arguments)
{
}

FunctionExpression::~FunctionExpression()
{
    for (Expression* arg : arguments)
    {
        delete arg;
    }
}

void FunctionExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& FunctionExpression::GetName() const
{
    return name;
}

const vector<Expression*>& FunctionExpression::GetArguments() const
{
    return arguments;
}

VariableDefinition::VariableDefinition(const string& name) : name(name)
{
}

const string& VariableDefinition::GetName() const
{
    return name;
}

FunctionDefinition::FunctionDefinition(const string& name,
                                       const vector<VariableDefinition*>& parameters,
                                       SyntaxTreeNode* code) :
    name(name),
    parameters(parameters),
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

void FunctionDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& FunctionDefinition::GetName() const
{
    return name;
}

const vector<VariableDefinition*>& FunctionDefinition::GetParameters() const
{
    return parameters;
}

SyntaxTreeNode* FunctionDefinition::GetCode() const
{
    return code;
}

ModuleDefinition::ModuleDefinition(const vector<FunctionDefinition*>& functionDefinitions) :
    functionDefinitions(functionDefinitions)
{
}

ModuleDefinition::~ModuleDefinition()
{
    for (FunctionDefinition* funcDef : functionDefinitions)
    {
        delete funcDef;
    }
}

void ModuleDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const vector<FunctionDefinition*>& ModuleDefinition::GetFunctionDefinitions() const
{
    return functionDefinitions;
}
} // namespace SyntaxTree
