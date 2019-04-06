#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"

using namespace std;

namespace SyntaxTree
{
Expression::Expression() :
    type(EType::eUnknown)
{
}

EType Expression::GetType() const
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

BoolLiteralExpression::BoolLiteralExpression(const string& value) :
    value(value)
{
}

void BoolLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& BoolLiteralExpression::GetValue() const
{
    return value;
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

VariableDefinition::VariableDefinition(const string& name, EType type) :
    name(name),
    type(type)
{
}

const string& VariableDefinition::GetName() const
{
    return name;
}

EType VariableDefinition::GetType() const
{
    return type;
}

FunctionDefinition::FunctionDefinition(const string& name,
                                       const vector<VariableDefinition*>& parameters,
                                       EType returnType,
                                       Expression* code) :
    name(name),
    parameters(parameters),
    returnType(returnType),
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

EType FunctionDefinition::GetReturnType() const
{
    return returnType;
}

Expression* FunctionDefinition::GetCode() const
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