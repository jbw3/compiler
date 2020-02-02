#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "utils.h"
#include <limits>

using namespace std;

namespace SyntaxTree
{
Expression::Expression() :
    type(nullptr),
    isAssignable(false),
    accessType(eLoad)
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

bool Expression::GetIsAssignable() const
{
    return isAssignable;
}

void Expression::SetIsAssignable(bool newIsAssignable)
{
    isAssignable = newIsAssignable;
}

Expression::EAccessType Expression::GetAccessType() const
{
    return accessType;
}

void Expression::SetAccessType(EAccessType newAccessType)
{
    accessType = newAccessType;
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

StringLiteralExpression::StringLiteralExpression(const vector<char> characters) :
    characters(characters)
{
}

void StringLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const vector<char>& StringLiteralExpression::GetCharacters() const
{
    return characters;
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

bool BinaryExpression::IsAssignment(EOperator op)
{
    switch (op)
    {
        case BinaryExpression::eEqual:
        case BinaryExpression::eNotEqual:
        case BinaryExpression::eLessThan:
        case BinaryExpression::eLessThanOrEqual:
        case BinaryExpression::eGreaterThan:
        case BinaryExpression::eGreaterThanOrEqual:
        case BinaryExpression::eAdd:
        case BinaryExpression::eSubtract:
        case BinaryExpression::eMultiply:
        case BinaryExpression::eDivide:
        case BinaryExpression::eRemainder:
        case BinaryExpression::eShiftLeft:
        case BinaryExpression::eShiftRightArithmetic:
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eBitwiseAnd:
        case BinaryExpression::eBitwiseXor:
        case BinaryExpression::eBitwiseOr:
        case BinaryExpression::eLogicalAnd:
        case BinaryExpression::eLogicalOr:
            return false;
        case BinaryExpression::eAssign:
        case BinaryExpression::eAddAssign:
        case BinaryExpression::eSubtractAssign:
        case BinaryExpression::eMultiplyAssign:
        case BinaryExpression::eDivideAssign:
        case BinaryExpression::eRemainderAssign:
        case BinaryExpression::eShiftLeftAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eBitwiseAndAssign:
        case BinaryExpression::eBitwiseXorAssign:
        case BinaryExpression::eBitwiseOrAssign:
            return true;
    }
}

bool BinaryExpression::IsComputationAssignment(EOperator op)
{
    switch (op)
    {
        case BinaryExpression::eEqual:
        case BinaryExpression::eNotEqual:
        case BinaryExpression::eLessThan:
        case BinaryExpression::eLessThanOrEqual:
        case BinaryExpression::eGreaterThan:
        case BinaryExpression::eGreaterThanOrEqual:
        case BinaryExpression::eAdd:
        case BinaryExpression::eSubtract:
        case BinaryExpression::eMultiply:
        case BinaryExpression::eDivide:
        case BinaryExpression::eRemainder:
        case BinaryExpression::eShiftLeft:
        case BinaryExpression::eShiftRightArithmetic:
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eBitwiseAnd:
        case BinaryExpression::eBitwiseXor:
        case BinaryExpression::eBitwiseOr:
        case BinaryExpression::eLogicalAnd:
        case BinaryExpression::eLogicalOr:
        case BinaryExpression::eAssign:
            return false;
        case BinaryExpression::eAddAssign:
        case BinaryExpression::eSubtractAssign:
        case BinaryExpression::eMultiplyAssign:
        case BinaryExpression::eDivideAssign:
        case BinaryExpression::eRemainderAssign:
        case BinaryExpression::eShiftLeftAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eBitwiseAndAssign:
        case BinaryExpression::eBitwiseXorAssign:
        case BinaryExpression::eBitwiseOrAssign:
            return true;
    }
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

VariableDeclaration::VariableDeclaration(const string& name, const TypeInfo* type) :
    name(name),
    type(type)
{
}

void VariableDeclaration::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& VariableDeclaration::GetName() const
{
    return name;
}

const TypeInfo* VariableDeclaration::GetType() const
{
    return type;
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

FunctionDeclaration::FunctionDeclaration(const std::string& name,
                                         const VariableDeclarations& parameters,
                                         const TypeInfo* returnType) :
    name(name),
    parameters(parameters),
    returnType(returnType)
{
}

FunctionDeclaration::~FunctionDeclaration()
{
    deletePointerContainer(parameters);
}

const string& FunctionDeclaration::GetName() const
{
    return name;
}

const VariableDeclarations& FunctionDeclaration::GetParameters() const
{
    return parameters;
}

const TypeInfo* FunctionDeclaration::GetReturnType() const
{
    return returnType;
}

ExternFunctionDeclaration::ExternFunctionDeclaration(FunctionDeclaration* declaration) :
    declaration(declaration)
{
}

ExternFunctionDeclaration::~ExternFunctionDeclaration()
{
    delete declaration;
}

void ExternFunctionDeclaration::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const FunctionDeclaration* ExternFunctionDeclaration::GetDeclaration() const
{
    return declaration;
}

FunctionDefinition::FunctionDefinition(FunctionDeclaration* declaration,
                                       Expression* expression) :
    declaration(declaration),
    expression(expression)
{
}

FunctionDefinition::~FunctionDefinition()
{
    delete declaration;
    delete expression;
}

void FunctionDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const FunctionDeclaration* FunctionDefinition::GetDeclaration() const
{
    return declaration;
}

Expression* FunctionDefinition::GetExpression() const
{
    return expression;
}

ModuleDefinition::ModuleDefinition(const vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                                   const vector<FunctionDefinition*>& functionDefinitions) :
    externFunctionDeclarations(externFunctionDeclarations),
    functionDefinitions(functionDefinitions)
{
}

ModuleDefinition::~ModuleDefinition()
{
    deletePointerContainer(externFunctionDeclarations);
    deletePointerContainer(functionDefinitions);
}

void ModuleDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const vector<ExternFunctionDeclaration*>& ModuleDefinition::GetExternFunctionDeclarations() const
{
    return externFunctionDeclarations;
}

const vector<FunctionDefinition*>& ModuleDefinition::GetFunctionDefinitions() const
{
    return functionDefinitions;
}
} // namespace SyntaxTree
