#include "SyntaxTree.h"
#include "SyntaxTreeVisitor.h"
#include "keywords.h"
#include "utils.h"
#include <cassert>
#include <limits>

using namespace std;

namespace SyntaxTree
{
Expression::Expression() :
    type(nullptr),
    isStorage(false),
    accessType(eValue)
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

bool Expression::GetIsStorage() const
{
    return isStorage;
}

void Expression::SetIsStorage(bool newIsStorage)
{
    isStorage = newIsStorage;
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

BoolLiteralExpression::BoolLiteralExpression(const Token* token) :
    token(token)
{
}

void BoolLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
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

ArraySizeValueExpression::ArraySizeValueExpression(
    Expression* sizeExpression, Expression* valueExpression,
    const Token* startToken, const Token* endToken) :
    startToken(startToken),
    endToken(endToken),
    sizeExpression(sizeExpression),
    valueExpression(valueExpression)
{
}

ArraySizeValueExpression::~ArraySizeValueExpression()
{
    delete sizeExpression;
    delete valueExpression;
}

void ArraySizeValueExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

ArrayMultiValueExpression::ArrayMultiValueExpression(
    const Expressions& expressions,
    const Token* startToken, const Token* endToken) :
    startToken(startToken),
    endToken(endToken),
    expressions(expressions)
{
}

ArrayMultiValueExpression::~ArrayMultiValueExpression()
{
    deletePointerContainer(expressions);
}

void ArrayMultiValueExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

BlockExpression::BlockExpression(const SyntaxTreeNodes& statements,
                                 const Token* startToken, const Token* endToken) :
    startToken(startToken),
    endToken(endToken),
    statements(statements)
{
}

BlockExpression::~BlockExpression()
{
    deletePointerContainer(statements);
}

void BlockExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
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
            return "-=";
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
        case BinaryExpression::eClosedRange:
            return "..";
        case BinaryExpression::eHalfOpenRange:
            return "..<";
        case BinaryExpression::eSubscript:
            return "[]";
    }

    // we should never get here
    return "";
}

unsigned BinaryExpression::GetPrecedence(EOperator op)
{
    switch (op)
    {
        case BinaryExpression::eSubscript:
            return 0;
        case BinaryExpression::eMultiply:
        case BinaryExpression::eDivide:
        case BinaryExpression::eRemainder:
            return 1;
        case BinaryExpression::eAdd:
        case BinaryExpression::eSubtract:
            return 2;
        case BinaryExpression::eShiftLeft:
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eShiftRightArithmetic:
            return 3;
        case BinaryExpression::eBitwiseAnd:
            return 4;
        case BinaryExpression::eBitwiseXor:
            return 5;
        case BinaryExpression::eBitwiseOr:
            return 6;
        case BinaryExpression::eClosedRange:
        case BinaryExpression::eHalfOpenRange:
            return 7;
        case BinaryExpression::eEqual:
        case BinaryExpression::eNotEqual:
        case BinaryExpression::eLessThan:
        case BinaryExpression::eLessThanOrEqual:
        case BinaryExpression::eGreaterThan:
        case BinaryExpression::eGreaterThanOrEqual:
            return 8;
        case BinaryExpression::eLogicalAnd:
        case BinaryExpression::eLogicalOr:
            return 9;
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
            return 10;
    }
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
        case BinaryExpression::eClosedRange:
        case BinaryExpression::eHalfOpenRange:
        case BinaryExpression::eSubscript:
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
        case BinaryExpression::eClosedRange:
        case BinaryExpression::eHalfOpenRange:
        case BinaryExpression::eSubscript:
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

string UnaryExpression::GetOperatorString(EOperator op)
{
    switch (op)
    {
        case UnaryExpression::eNegative:
            return "-";
        case UnaryExpression::eComplement:
            return "!";
        case UnaryExpression::eAddressOf:
            return "&";
        case UnaryExpression::eDereference:
            return "*";
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

VariableExpression::VariableExpression(const string& name, const Token* token) :
    token(token),
    name(name)
{
}

void VariableExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

CastExpression::CastExpression(Expression* subExpression, const Token* castToken,
                               const vector<const Token*>& castTypeNameTokens) :
    subExpression(subExpression),
    castToken(castToken),
    castTypeNameTokens(castTypeNameTokens)
{
}

CastExpression::~CastExpression()
{
    delete subExpression;
}

void CastExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

ImplicitCastExpression::ImplicitCastExpression(Expression* subExpression) :
    subExpression(subExpression)
{
}

ImplicitCastExpression::~ImplicitCastExpression()
{
    delete subExpression;
}

void ImplicitCastExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

FunctionExpression::FunctionExpression(const string& name, const Expressions& arguments, const Token* nameToken) :
    nameToken(nameToken),
    functionDeclaration(nullptr),
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

MemberExpression::MemberExpression(Expression* subExpr, const std::string& memberName,
                                   const Token* opToken, const Token* memberNameToken) :
    opToken(opToken),
    memberNameToken(memberNameToken),
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

BranchExpression::BranchExpression(Expression* ifCondition,
                                   Expression* ifExpression,
                                   Expression* elseExpression,
                                   const Token* ifToken,
                                   const Token* elseToken) :
    ifToken(ifToken),
    elseToken(elseToken),
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

VariableDeclaration::VariableDeclaration(const string& name, BinaryExpression* assignmentExpression,
                                         const Token* nameToken, const vector<const Token*>& typeNameTokens) :
    nameToken(nameToken),
    typeNameTokens(typeNameTokens),
    name(name),
    variableType(nullptr),
    assignmentExpression(assignmentExpression)
{
}

void VariableDeclaration::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

WhileLoop::WhileLoop(Expression* condition, BlockExpression* expression, const Token* whileToken) :
    condition(condition),
    expression(expression),
    whileToken(whileToken)
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

ForLoop::ForLoop(const string& variableName, const string& indexName,
                 Expression* iterExpression,
                 BlockExpression* expression, const Token* forToken,
                 const Token* inToken, const Token* variableNameToken,
                 const vector<const Token*>& variableTypeNameTokens,
                 const Token* indexNameToken,
                 const vector<const Token*>& indexTypeNameTokens) :
    variableName(variableName),
    indexName(indexName),
    iterExpression(iterExpression),
    expression(expression),
    forToken(forToken),
    inToken(inToken),
    variableNameToken(variableNameToken),
    variableTypeNameTokens(variableTypeNameTokens),
    indexNameToken(indexNameToken),
    indexTypeNameTokens(indexTypeNameTokens)
{
}

ForLoop::~ForLoop()
{
    delete expression;
    delete iterExpression;
}

void ForLoop::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

LoopControl::LoopControl(const Token* token) :
    token(token)
{
}

void LoopControl::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

Return::Return(const Token* token, Expression* expression) :
    token(token),
    expression(expression)
{
}

Return::~Return()
{
    delete expression;
}

void Return::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

Parameter::Parameter(const string& name, const Token* nameToken,
                     const vector<const Token*>& typeNameTokens) :
    nameToken(nameToken),
    typeNameTokens(typeNameTokens),
    name(name),
    type(nullptr)
{
}

FunctionDeclaration::FunctionDeclaration(const string& name,
                                         const Parameters& parameters,
                                         const Token* nameToken,
                                         const vector<const Token*>& returnTypeNameTokens) :
    nameToken(nameToken),
    returnTypeNameTokens(returnTypeNameTokens),
    name(name),
    parameters(parameters),
    returnType(nullptr)
{
}

FunctionDeclaration::~FunctionDeclaration()
{
    deletePointerContainer(parameters);
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

FunctionDefinition::FunctionDefinition(FunctionDeclaration* declaration,
                                       Expression* expression) :
    declaration(declaration),
    expression(expression),
    endsWithReturnStatement(false)
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

MemberDefinition::MemberDefinition(const string& name, const Token* nameToken,
                                   const vector<const Token*>& typeNameTokens) :
    nameToken(nameToken),
    typeNameTokens(typeNameTokens),
    name(name)
{
}

StructDefinition::StructDefinition(const string& name, const vector<MemberDefinition*>& members,
                                   const Token* nameToken, unsigned fileId) :
    nameToken(nameToken),
    name(name),
    members(members),
    type(nullptr),
    fileId(fileId)
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

MemberInitialization::MemberInitialization(const string& name, Expression* expression,
                                           const Token* nameToken) :
    name(name),
    expression(expression),
    nameToken(nameToken)
{
}

MemberInitialization::~MemberInitialization()
{
    delete expression;
}

StructInitializationExpression::StructInitializationExpression(const string& structName, const vector<MemberInitialization*>& memberInitializations,
                                                               const Token* structNameToken) :
    structName(structName),
    memberInitializations(memberInitializations),
    structNameToken(structNameToken)
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

ModuleDefinition::ModuleDefinition(unsigned fileId,
                                   const vector<StructDefinition*>& structDefinitions,
                                   const vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                                   const vector<FunctionDefinition*>& functionDefinitions) :
    fileId(fileId),
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

Modules::~Modules()
{
    deletePointerContainer(modules);
}

void Modules::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}
} // namespace SyntaxTree
