#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "Token.h"
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

NumericExpression::NumericExpression(int64_t value, const Token* token) :
    token(token),
    value(value)
{
}

void NumericExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

int64_t NumericExpression::GetValue() const
{
    return value;
}

const Token* NumericExpression::GetToken() const
{
    return token;
}

unsigned NumericExpression::GetMinSignedSize() const
{
    unsigned numBits = 0;
    if (value >= numeric_limits<int8_t>::min() && value <= numeric_limits<int8_t>::max())
    {
        numBits = 8;
    }
    else if (value >= numeric_limits<int16_t>::min() && value <= numeric_limits<int16_t>::max())
    {
        numBits = 16;
    }
    else if (value >= numeric_limits<int32_t>::min() && value <= numeric_limits<int32_t>::max())
    {
        numBits = 32;
    }
    else if (value >= numeric_limits<int64_t>::min() && value <= numeric_limits<int64_t>::max())
    {
        numBits = 64;
    }

    return numBits;
}

unsigned NumericExpression::GetMinUnsignedSize() const
{
    unsigned numBits = 0;
    if (value <= numeric_limits<uint8_t>::max())
    {
        numBits = 8;
    }
    else if (value <= numeric_limits<uint16_t>::max())
    {
        numBits = 16;
    }
    else if (value <= numeric_limits<uint32_t>::max())
    {
        numBits = 32;
    }
    else if (value <= numeric_limits<uint64_t>::max())
    {
        numBits = 64;
    }

    return numBits;
}

BoolLiteralExpression::BoolLiteralExpression(bool value, const Token* token) :
    token(token),
    value(value)
{
}

void BoolLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

bool BoolLiteralExpression::GetValue() const
{
    return value;
}

const Token* BoolLiteralExpression::GetToken() const
{
    return token;
}

StringLiteralExpression::StringLiteralExpression(const vector<char> characters, const Token* token) :
    token(token),
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

const Token* StringLiteralExpression::GetToken() const
{
    return token;
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

std::string BinaryExpression::GetOperatorString(EOperator op)
{
    switch (op)
    {
        case BinaryExpression::eEqual:
            return "==";
        case BinaryExpression::eNotEqual:
            return "!=";
        case BinaryExpression::eLessThan:
            return "<";
        case BinaryExpression::eLessThanOrEqual:
            return "<=";
        case BinaryExpression::eGreaterThan:
            return ">";
        case BinaryExpression::eGreaterThanOrEqual:
            return ">=";
        case BinaryExpression::eAdd:
            return "+";
        case BinaryExpression::eSubtract:
            return "-";
        case BinaryExpression::eMultiply:
            return "*";
        case BinaryExpression::eDivide:
            return "/";
        case BinaryExpression::eRemainder:
            return "%";
        case BinaryExpression::eShiftLeft:
            return "<<";
        case BinaryExpression::eShiftRightLogical:
            return ">>";
        case BinaryExpression::eShiftRightArithmetic:
            return ">>>";
        case BinaryExpression::eBitwiseAnd:
            return "&";
        case BinaryExpression::eBitwiseXor:
            return "^";
        case BinaryExpression::eBitwiseOr:
            return "|";
        case BinaryExpression::eLogicalAnd:
            return "&&";
        case BinaryExpression::eLogicalOr:
            return "||";
        case BinaryExpression::eAssign:
            return "=";
        case BinaryExpression::eAddAssign:
            return "+=";
        case BinaryExpression::eSubtractAssign:
            return "==";
        case BinaryExpression::eMultiplyAssign:
            return "*=";
        case BinaryExpression::eDivideAssign:
            return "/=";
        case BinaryExpression::eRemainderAssign:
            return "%=";
        case BinaryExpression::eShiftLeftAssign:
            return "<<=";
        case BinaryExpression::eShiftRightArithmeticAssign:
            return ">>=";
        case BinaryExpression::eShiftRightLogicalAssign:
            return ">>>=";
        case BinaryExpression::eBitwiseAndAssign:
            return "&=";
        case BinaryExpression::eBitwiseXorAssign:
            return "^=";
        case BinaryExpression::eBitwiseOrAssign:
            return "|=";
    }

    // we should never get here
    return "";
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
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eShiftRightArithmetic:
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
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
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
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eShiftRightArithmetic:
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
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
        case BinaryExpression::eBitwiseAndAssign:
        case BinaryExpression::eBitwiseXorAssign:
        case BinaryExpression::eBitwiseOrAssign:
            return true;
    }
}

BinaryExpression::BinaryExpression(EOperator op, Expression* left, Expression* right, const Token* opToken) :
    opToken(opToken),
    left(left),
    right(right),
    op(op)
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

const Token* BinaryExpression::GetOperatorToken() const
{
    return opToken;
}

string UnaryExpression::GetOperatorString(EOperator op)
{
    switch (op)
    {
        case UnaryExpression::eNegative:
            return "-";
        case UnaryExpression::eComplement:
            return "!";
    }

    // we should never get here
    return "";
}

UnaryExpression::UnaryExpression(EOperator op, Expression* subExpr, const Token* opToken) :
    opToken(opToken),
    subExpression(subExpr),
    op(op)
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

const Token* UnaryExpression::GetOperatorToken() const
{
    return opToken;
}

VariableExpression::VariableExpression(const string& name, const Token* token) :
    token(token),
    name(name)
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

const Token* VariableExpression::GetToken() const
{
    return token;
}

FunctionExpression::FunctionExpression(const string& name, const Expressions& arguments, const Token* nameToken) :
    nameToken(nameToken),
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

const Token* FunctionExpression::GetNameToken() const
{
    return nameToken;
}

MemberExpression::MemberExpression(Expression* subExpr, const std::string& memberName) :
    subExpression(subExpr),
    memberName(memberName)
{
}

MemberExpression::~MemberExpression()
{
    delete subExpression;
}

void MemberExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

void MemberExpression::SetAccessType(EAccessType newAccessType)
{
    Expression::SetAccessType(newAccessType);
    subExpression->SetAccessType(newAccessType);
}

Expression* MemberExpression::GetSubExpression() const
{
    return subExpression;
}

const string& MemberExpression::GetMemberName() const
{
    return memberName;
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

VariableDeclaration::VariableDeclaration(const string& name, const string& typeName, BinaryExpression* assignmentExpression) :
    name(name),
    typeName(typeName),
    variableType(nullptr),
    assignmentExpression(assignmentExpression)
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

const string& VariableDeclaration::GetTypeName() const
{
    return typeName;
}

const TypeInfo* VariableDeclaration::GetVariableType() const
{
    return variableType;
}

void VariableDeclaration::SetVariableType(const TypeInfo* newType)
{
    variableType = newType;
}

BinaryExpression* VariableDeclaration::GetAssignmentExpression() const
{
    return assignmentExpression;
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

Parameter::Parameter(const string& name, const string& typeName,
                     const Token* nameToken, const Token* typeNameToken) :
    nameToken(nameToken),
    typeNameToken(typeNameToken),
    name(name),
    typeName(typeName),
    type(nullptr)
{
}

const string& Parameter::GetName() const
{
    return name;
}

const string& Parameter::GetTypeName() const
{
    return typeName;
}

const TypeInfo* Parameter::GetType() const
{
    return type;
}

void Parameter::SetType(const TypeInfo* newType)
{
    type = newType;
}

const Token* Parameter::GetNameToken() const
{
    return nameToken;
}

const Token* Parameter::GetTypeNameToken() const
{
    return typeNameToken;
}

FunctionDeclaration::FunctionDeclaration(const std::string& name,
                                         const Parameters& parameters,
                                         const std::string& returnTypeName,
                                         const Token* nameToken,
                                         const Token* returnTypeNameToken) :
    nameToken(nameToken),
    returnTypeNameToken(returnTypeNameToken),
    name(name),
    parameters(parameters),
    returnTypeName(returnTypeName),
    returnType(nullptr)
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

const Parameters& FunctionDeclaration::GetParameters() const
{
    return parameters;
}

const string& FunctionDeclaration::GetReturnTypeName() const
{
    return returnTypeName;
}

const TypeInfo* FunctionDeclaration::GetReturnType() const
{
    return returnType;
}

void FunctionDeclaration::SetReturnType(const TypeInfo* newType)
{
    returnType = newType;
}

const Token* FunctionDeclaration::GetNameToken() const
{
    return nameToken;
}

const Token* FunctionDeclaration::GetReturnTypeNameToken() const
{
    return returnTypeNameToken;
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

FunctionDeclaration* ExternFunctionDeclaration::GetDeclaration() const
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

FunctionDeclaration* FunctionDefinition::GetDeclaration() const
{
    return declaration;
}

Expression* FunctionDefinition::GetExpression() const
{
    return expression;
}

MemberDefinition::MemberDefinition(const std::string& name, const std::string& typeName) :
    name(name),
    typeName(typeName)
{
}

const std::string& MemberDefinition::GetName() const
{
    return name;
}

const std::string& MemberDefinition::GetTypeName() const
{
    return typeName;
}

StructDefinition::StructDefinition(const string& name, const vector<MemberDefinition*>& members) :
    name(name),
    members(members),
    type(nullptr)
{
}

StructDefinition::~StructDefinition()
{
    deletePointerContainer(members);
}

void StructDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& StructDefinition::GetName() const
{
    return name;
}

const vector<MemberDefinition*>& StructDefinition::GetMembers() const
{
    return members;
}

const TypeInfo* StructDefinition::GetType() const
{
    return type;
}

void StructDefinition::SetType(const TypeInfo* newType)
{
    type = newType;
}

MemberInitialization::MemberInitialization(const string& name, Expression* expression) :
    name(name),
    expression(expression)
{
}

MemberInitialization::~MemberInitialization()
{
    delete expression;
}

const string& MemberInitialization::GetName() const
{
    return name;
}

Expression* MemberInitialization::GetExpression() const
{
    return expression;
}

StructInitializationExpression::StructInitializationExpression(const string& structName, const vector<MemberInitialization*>& memberInitializations) :
    structName(structName),
    memberInitializations(memberInitializations)
{
}

StructInitializationExpression::~StructInitializationExpression()
{
    deletePointerContainer(memberInitializations);
}

void StructInitializationExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const string& StructInitializationExpression::GetStructName() const
{
    return structName;
}

const vector<MemberInitialization*>& StructInitializationExpression::GetMemberInitializations() const
{
    return memberInitializations;
}

ModuleDefinition::ModuleDefinition(const vector<StructDefinition*>& structDefinitions,
                                   const vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                                   const vector<FunctionDefinition*>& functionDefinitions) :
    structDefinitions(structDefinitions),
    externFunctionDeclarations(externFunctionDeclarations),
    functionDefinitions(functionDefinitions)
{
}

ModuleDefinition::~ModuleDefinition()
{
    deletePointerContainer(structDefinitions);
    deletePointerContainer(externFunctionDeclarations);
    deletePointerContainer(functionDefinitions);
}

void ModuleDefinition::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

void ModuleDefinition::SwapStructDefinitions(vector<StructDefinition*>& newStructDefinitions)
{
    structDefinitions.swap(newStructDefinitions);
}

const vector<StructDefinition*>& ModuleDefinition::GetStructDefinitions() const
{
    return structDefinitions;
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
