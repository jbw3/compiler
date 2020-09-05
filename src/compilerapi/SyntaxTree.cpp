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

BlockExpression::BlockExpression(const Expressions& expressions,
                                 const Token* startToken, const Token* endToken) :
    startToken(startToken),
    endToken(endToken),
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

const Token* BlockExpression::GetStartToken() const
{
    return startToken;
}

const Token* BlockExpression::GetEndToken() const
{
    return endToken;
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
        case BinaryExpression::eInclusiveRange:
            return "..";
        case BinaryExpression::eExclusiveRange:
            return "..<";
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
        case BinaryExpression::eInclusiveRange:
        case BinaryExpression::eExclusiveRange:
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
        case BinaryExpression::eInclusiveRange:
        case BinaryExpression::eExclusiveRange:
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

const FunctionDeclaration* FunctionExpression::GetFunctionDeclaration() const
{
    return functionDeclaration;
}

void FunctionExpression::SetFunctionDeclaration(const FunctionDeclaration* decl)
{
    functionDeclaration = decl;
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

Expression* MemberExpression::GetSubExpression() const
{
    return subExpression;
}

const string& MemberExpression::GetMemberName() const
{
    return memberName;
}

const Token* MemberExpression::GetOperatorToken() const
{
    return opToken;
}

const Token* MemberExpression::GetMemberNameToken() const
{
    return memberNameToken;
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

const string& VariableDeclaration::GetName() const
{
    return name;
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

const Token* VariableDeclaration::GetNameToken() const
{
    return nameToken;
}

const vector<const Token*>& VariableDeclaration::GetTypeNameTokens() const
{
    return typeNameTokens;
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

Expression* WhileLoop::GetCondition() const
{
    return condition;
}

BlockExpression* WhileLoop::GetExpression() const
{
    return expression;
}

const Token* WhileLoop::GetWhileToken() const
{
    return whileToken;
}

ForLoop::ForLoop(const string& variableName, Expression* iterExpression,
                 BlockExpression* expression, const Token* forToken,
                 const Token* inToken, const Token* variableNameToken,
                 const vector<const Token*>& variableTypeNameTokens) :
    variableName(variableName),
    iterExpression(iterExpression),
    expression(expression),
    forToken(forToken),
    inToken(inToken),
    variableNameToken(variableNameToken),
    variableTypeNameTokens(variableTypeNameTokens)
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

const string& ForLoop::GetVariableName() const
{
    return variableName;
}

const TypeInfo* ForLoop::GetVariableType() const
{
    return variableType;
}

void ForLoop::SetVariableType(const TypeInfo* newType)
{
    variableType = newType;
}

Expression* ForLoop::GetIterExpression()
{
    return iterExpression;
}

BlockExpression* ForLoop::GetExpression()
{
    return expression;
}

const Token* ForLoop::GetForToken() const
{
    return forToken;
}

const Token* ForLoop::GetInToken() const
{
    return inToken;
}

const Token* ForLoop::GetVariableNameToken() const
{
    return variableNameToken;
}

const vector<const Token*>& ForLoop::GetVariableTypeNameTokens() const
{
    return variableTypeNameTokens;
}

Parameter::Parameter(const string& name, const Token* nameToken,
                     const vector<const Token*>& typeNameTokens) :
    nameToken(nameToken),
    typeNameTokens(typeNameTokens),
    name(name),
    type(nullptr)
{
}

const string& Parameter::GetName() const
{
    return name;
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

const vector<const Token*>& Parameter::GetTypeNameTokens() const
{
    return typeNameTokens;
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

const string& FunctionDeclaration::GetName() const
{
    return name;
}

const Parameters& FunctionDeclaration::GetParameters() const
{
    return parameters;
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

const vector<const Token*>& FunctionDeclaration::GetReturnTypeNameTokens() const
{
    return returnTypeNameTokens;
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

MemberDefinition::MemberDefinition(const string& name, const Token* nameToken,
                                   const vector<const Token*>& typeNameTokens) :
    nameToken(nameToken),
    typeNameTokens(typeNameTokens),
    name(name)
{
}

const std::string& MemberDefinition::GetName() const
{
    return name;
}

const Token* MemberDefinition::GetNameToken() const
{
    return nameToken;
}

const vector<const Token*>& MemberDefinition::GetTypeNameTokens() const
{
    return typeNameTokens;
}

StructDefinition::StructDefinition(const string& name, const vector<MemberDefinition*>& members,
                                   const Token* nameToken) :
    nameToken(nameToken),
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

const Token* StructDefinition::GetNameToken() const
{
    return nameToken;
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

const string& MemberInitialization::GetName() const
{
    return name;
}

Expression* MemberInitialization::GetExpression() const
{
    return expression;
}

const Token* MemberInitialization::GetNameToken() const
{
    return nameToken;
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

const string& StructInitializationExpression::GetStructName() const
{
    return structName;
}

const vector<MemberInitialization*>& StructInitializationExpression::GetMemberInitializations() const
{
    return memberInitializations;
}

const Token* StructInitializationExpression::GetStructNameToken() const
{
    return structNameToken;
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
