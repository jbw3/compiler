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
    flags(CONST_VALUE_MASK)
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
    return (flags & F_IS_STORAGE) != 0;
}

void Expression::SetIsStorage(bool newIsStorage)
{
    if (newIsStorage)
    {
        flags |= F_IS_STORAGE;
    }
    else
    {
        flags &= ~F_IS_STORAGE;
    }
}

Expression::EAccessType Expression::GetAccessType() const
{
    return static_cast<EAccessType>((flags & F_ACCESS_TYPE) >> 31);
}

void Expression::SetAccessType(EAccessType newAccessType)
{
    if (newAccessType == eValue)
    {
        flags &= ~F_ACCESS_TYPE;
    }
    else
    {
        flags |= F_ACCESS_TYPE;
    }
}

unsigned Expression::GetConstantValueIndex() const
{
    return flags & CONST_VALUE_MASK;
}

void Expression::SetConstantValueIndex(unsigned newConstantValueIndex)
{
    flags &= ~CONST_VALUE_MASK;
    flags |= newConstantValueIndex & CONST_VALUE_MASK;
}

bool Expression::GetIsConstant() const
{
    // if the constant value index is its max value, this is not a constant
    return (flags & CONST_VALUE_MASK) != CONST_VALUE_MASK;
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
    uint64_t unsignedValue = static_cast<uint64_t>(value);
    if (unsignedValue <= numeric_limits<uint8_t>::max())
    {
        numBits = 8;
    }
    else if (unsignedValue <= numeric_limits<uint16_t>::max())
    {
        numBits = 16;
    }
    else if (unsignedValue <= numeric_limits<uint32_t>::max())
    {
        numBits = 32;
    }
    else if (unsignedValue <= numeric_limits<uint64_t>::max())
    {
        numBits = 64;
    }

    return numBits;
}

FloatLiteralExpression::FloatLiteralExpression(double value, const Token* token) :
    token(token),
    value(value)
{
}

void FloatLiteralExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
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

const char* BinaryExpression::GetOperatorString(EOperator op)
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

    // we should never get here
    return static_cast<unsigned>(-1);
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

    // we should never get here
    return false;
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

    // we should never get here
    return false;
}

BinaryExpression::BinaryExpression(EOperator op, Expression* left, Expression* right, const Token* opToken, const Token* opToken2) :
    opToken(opToken),
    opToken2(opToken2),
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

const char* UnaryExpression::GetOperatorString(EOperator op)
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
        case UnaryExpression::eArrayOf:
            return "[]";
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

IdentifierExpression::IdentifierExpression(ROString name, const Token* token) :
    token(token),
    name(name)
{
}

void IdentifierExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

CastExpression::CastExpression(Expression* typeExpression, Expression* subExpression,
                               const Token* castToken,
                               const Token* openParToken,
                               const Token* closeParToken) :
    typeExpression(typeExpression),
    subExpression(subExpression),
    castToken(castToken),
    openParToken(openParToken),
    closeParToken(closeParToken)
{
}

CastExpression::~CastExpression()
{
    delete typeExpression;
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

FunctionCallExpression::FunctionCallExpression(Expression* functionExpr, const Expressions& arguments,
                                               const Token* openParToken, const Token* closeParToken) :
    openParToken(openParToken),
    closeParToken(closeParToken),
    functionType(nullptr),
    functionExpression(functionExpr),
    arguments(arguments)
{
}

FunctionCallExpression::~FunctionCallExpression()
{
    for (Expression* arg : arguments)
    {
        delete arg;
    }

    delete functionExpression;
}

void FunctionCallExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

MemberExpression::MemberExpression(Expression* subExpr, ROString memberName,
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

ConstantDeclaration::ConstantDeclaration(ROString name, BinaryExpression* assignmentExpression,
                                         Expression* typeExpression, const Token* nameToken) :
    nameToken(nameToken),
    name(name),
    constantType(nullptr),
    assignmentExpression(assignmentExpression),
    typeExpression(typeExpression)
{
}

ConstantDeclaration::~ConstantDeclaration()
{
    delete assignmentExpression;
    delete typeExpression;
}

void ConstantDeclaration::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

VariableDeclaration::VariableDeclaration(ROString name, BinaryExpression* assignmentExpression,
                                         Expression* typeExpression, const Token* nameToken) :
    nameToken(nameToken),
    name(name),
    variableType(nullptr),
    assignmentExpression(assignmentExpression),
    typeExpression(typeExpression)
{
}

VariableDeclaration::~VariableDeclaration()
{
    delete assignmentExpression;
    delete typeExpression;
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

ForLoop::ForLoop(ROString variableName,
                 Expression* varTypeExpression,
                 ROString indexName,
                 Expression* indexTypeExpression,
                 Expression* iterExpression,
                 BlockExpression* expression, const Token* forToken,
                 const Token* inToken, const Token* variableNameToken,
                 const Token* indexNameToken) :
    variableName(variableName),
    varTypeExpression(varTypeExpression),
    indexName(indexName),
    indexTypeExpression(indexTypeExpression),
    iterExpression(iterExpression),
    expression(expression),
    forToken(forToken),
    inToken(inToken),
    variableNameToken(variableNameToken),
    indexNameToken(indexNameToken)
{
}

ForLoop::~ForLoop()
{
    delete expression;
    delete iterExpression;
    delete indexTypeExpression;
    delete varTypeExpression;
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

Parameter::Parameter(ROString name,
                     Expression* typeExpression,
                     const Token* nameToken) :
    nameToken(nameToken),
    name(name),
    typeExpression(typeExpression),
    type(nullptr)
{
}

Parameter::~Parameter()
{
    delete typeExpression;
}

FunctionDeclaration::FunctionDeclaration(ROString name,
                                         const Parameters& parameters,
                                         Expression* returnTypeExpression,
                                         const Token* nameToken) :
    nameToken(nameToken),
    name(name),
    parameters(parameters),
    returnTypeExpression(returnTypeExpression),
    returnType(nullptr)
{
}

FunctionDeclaration::~FunctionDeclaration()
{
    deletePointerContainer(parameters);
    delete returnTypeExpression;
}

FunctionTypeExpression::FunctionTypeExpression(
    const Expressions& paramTypeExpressions,
    const vector<ROString>& paramNames,
    Expression* returnTypeExpression,
    const Token* funToken,
    const Token* openParToken,
    const Token* closeParToken,
    const vector<const Token*> paramNameTokens
) :
    funToken(funToken),
    openParToken(openParToken),
    closeParToken(closeParToken),
    paramNameTokens(paramNameTokens),
    paramTypeExpressions(paramTypeExpressions),
    paramNames(paramNames),
    returnTypeExpression(returnTypeExpression)
{
}

FunctionTypeExpression::~FunctionTypeExpression()
{
    deletePointerContainer(paramTypeExpressions);
    delete returnTypeExpression;
}

void FunctionTypeExpression::Accept(SyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
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

MemberDefinition::MemberDefinition(ROString name, Expression* typeExpression,
                                   const Token* nameToken) :
    nameToken(nameToken),
    typeExpression(typeExpression),
    name(name)
{
}

MemberDefinition::~MemberDefinition()
{
    delete typeExpression;
}

StructDefinition::StructDefinition(ROString name, const vector<MemberDefinition*>& members,
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

StructDefinitionExpression::StructDefinitionExpression(const vector<MemberDefinition*>& members) :
    members(members)
{
}

StructDefinitionExpression::~StructDefinitionExpression()
{
    deletePointerContainer(members);
}

void StructDefinitionExpression::Accept(SyntaxTreeVisitor* visitor)
{
    // TODO
}

MemberInitialization::MemberInitialization(ROString name, Expression* expression,
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

StructInitializationExpression::StructInitializationExpression(ROString structName, const vector<MemberInitialization*>& memberInitializations,
                                                               const Token* structNameToken,
                                                               const Token* openBraceToken,
                                                               const Token* closeBraceToken) :
    structName(structName),
    memberInitializations(memberInitializations),
    structNameToken(structNameToken),
    openBraceToken(openBraceToken),
    closeBraceToken(closeBraceToken)
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
                                   const vector<ConstantDeclaration*> constantDeclarations,
                                   const vector<StructDefinition*>& structDefinitions,
                                   const vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                                   const vector<FunctionDefinition*>& functionDefinitions) :
    fileId(fileId),
    constantDeclarations(constantDeclarations),
    structDefinitions(structDefinitions),
    externFunctionDeclarations(externFunctionDeclarations),
    functionDefinitions(functionDefinitions)
{
}

ModuleDefinition::~ModuleDefinition()
{
    deletePointerContainer(constantDeclarations);
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
