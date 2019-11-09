#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "utils.h"
#include <limits>

using namespace std;

namespace SyntaxTree
{
Expression::Expression() :
    type(nullptr)
{
}

const TypeInfo* Expression::GetType() const
{
    return type;
}

void Expression::SetType(const TypeInfo* newType)
{
    type = newType;
}

UnitTypeLiteralExpression::UnitTypeLiteralExpression()
{
}

void UnitTypeLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
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

const TypeInfo* NumericExpression::GetMinSizeType() const
{
    const TypeInfo* type = nullptr;
    int64_t outNum = 0;
    bool ok = stringToInteger(number, outNum);
    if (ok)
    {
        if (outNum >= numeric_limits<int8_t>::min() && outNum <= numeric_limits<int8_t>::max())
        {
            type = TypeInfo::Int8Type;
        }
        else if (outNum >= numeric_limits<int16_t>::min() && outNum <= numeric_limits<int16_t>::max())
        {
            type = TypeInfo::Int16Type;
        }
        else if (outNum >= numeric_limits<int32_t>::min() && outNum <= numeric_limits<int32_t>::max())
        {
            type = TypeInfo::Int32Type;
        }
        else if (outNum >= numeric_limits<int64_t>::min() && outNum <= numeric_limits<int64_t>::max())
        {
            type = TypeInfo::Int64Type;
        }
    }

    return type;
}

BoolLiteralExpression* BoolLiteralExpression::CreateTrueExpression()
{
    BoolLiteralExpression* expr = new BoolLiteralExpression("true");
    expr->SetType(TypeInfo::BoolType);
    return expr;
}

BoolLiteralExpression* BoolLiteralExpression::CreateFalseExpression()
{
    BoolLiteralExpression* expr = new BoolLiteralExpression("false");
    expr->SetType(TypeInfo::BoolType);
    return expr;
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

BlockExpression::BlockExpression(const Expressions& expressions) :
    expressions(expressions)
{
}

BlockExpression::~BlockExpression()
{
    deletePointerContainer(expressions);
}

void BlockExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const Expressions& BlockExpression::GetExpressions() const
{
    return expressions;
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

UnaryExpression::UnaryExpression(EOperator op, Expression* subExpr) :
    op(op),
    subExpression(subExpr)
{
}

UnaryExpression::~UnaryExpression()
{
    delete subExpression;
}

void UnaryExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

UnaryExpression::EOperator UnaryExpression::GetOperator() const
{
    return op;
}

Expression* UnaryExpression::GetSubExpression() const
{
    return subExpression;
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

FunctionExpression::FunctionExpression(const string& name, const Expressions& arguments) :
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

const Expressions& FunctionExpression::GetArguments() const
{
    return arguments;
}

BranchExpression::BranchExpression(Expression* ifCondition,
                                   Expression* ifExpression,
                                   Expression* elseExpression) :
    ifCondition(ifCondition),
    ifExpression(ifExpression),
    elseExpression(elseExpression)
{
}

BranchExpression::~BranchExpression()
{
    delete ifCondition;
    delete ifExpression;
    delete elseExpression;
}

void BranchExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

Expression* BranchExpression::GetIfCondition() const
{
    return ifCondition;
}

Expression* BranchExpression::GetIfExpression() const
{
    return ifExpression;
}

Expression* BranchExpression::GetElseExpression() const
{
    return elseExpression;
}

VariableDefinition::VariableDefinition(const string& name, const TypeInfo* type) :
    name(name),
    type(type)
{
}

const string& VariableDefinition::GetName() const
{
    return name;
}

const TypeInfo* VariableDefinition::GetType() const
{
    return type;
}

Assignment::Assignment(const string& variableName, Expression* expression) :
    variableName(variableName),
    expression(expression)
{
}

Assignment::~Assignment()
{
    delete expression;
}

void Assignment::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& Assignment::GetVariableName() const
{
    return variableName;
}

Expression* Assignment::GetExpression() const
{
    return expression;
}

WhileLoop::WhileLoop(Expression* condition, Expression* expression) :
    condition(condition),
    expression(expression)
{
}

WhileLoop::~WhileLoop()
{
    delete condition;
    delete expression;
}

void WhileLoop::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

Expression* WhileLoop::GetCondition() const
{
    return condition;
}

Expression* WhileLoop::GetExpression() const
{
    return expression;
}

FunctionDefinition::FunctionDefinition(const string& name,
                                       const vector<VariableDefinition*>& parameters,
                                       const TypeInfo* returnType,
                                       const VariableDefinitions& variableDefinitions,
                                       Expression* expression) :
    name(name),
    parameters(parameters),
    returnType(returnType),
    variableDefinitions(variableDefinitions),
    expression(expression)
{
}

FunctionDefinition::~FunctionDefinition()
{
    for (VariableDefinition* param : parameters)
    {
        delete param;
    }
    for (VariableDefinition* varDef : variableDefinitions)
    {
        delete varDef;
    }
    delete expression;
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

const TypeInfo* FunctionDefinition::GetReturnType() const
{
    return returnType;
}

const VariableDefinitions& FunctionDefinition::GetVariableDefinitions() const
{
    return variableDefinitions;
}

Expression* FunctionDefinition::GetExpression() const
{
    return expression;
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
