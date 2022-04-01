#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <functional>
#include <vector>
#include "Token.h"
#include "TypeInfo.h"

class SyntaxTreeVisitor;

namespace SyntaxTree
{

class FunctionDeclaration;

class SyntaxTreeNode
{
public:
    virtual ~SyntaxTreeNode() = default;

    virtual void Accept(SyntaxTreeVisitor* visitor) = 0;
};

typedef std::vector<SyntaxTreeNode*> SyntaxTreeNodes;

class Expression : public SyntaxTreeNode
{
public:
    enum EAccessType
    {
        eValue,
        eAddress,
    };

    Expression();

    virtual ~Expression() = default;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

    bool GetIsStorage() const;

    void SetIsStorage(bool newIsStorage);

    EAccessType GetAccessType() const;

    virtual void SetAccessType(EAccessType newAccessType);

    unsigned GetConstantValueIndex() const;

    void SetConstantValueIndex(unsigned newConstantValueIndex);

    bool GetIsConstant() const;

private:
    static constexpr uint32_t F_ACCESS_TYPE    = 0x8000'0000;
    static constexpr uint32_t F_IS_STORAGE     = 0x4000'0000;
    static constexpr uint32_t CONST_VALUE_MASK = 0x00ff'ffff;

    const TypeInfo* type;
    uint32_t flags;
};

typedef std::vector<Expression*> Expressions;

class UnitTypeLiteralExpression : public Expression
{
public:
    UnitTypeLiteralExpression();

    virtual ~UnitTypeLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;
};

// TODO: Rename to NumericLiteralExpression
class NumericExpression : public Expression
{
public:
    NumericExpression(int64_t value, const Token* token);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    unsigned GetMinSignedSize() const;

    unsigned GetMinUnsignedSize() const;

    const Token* token;
    int64_t value;
};

class FloatLiteralExpression : public Expression
{
public:
    FloatLiteralExpression(double value, const Token* token);

    virtual ~FloatLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
    double value;
};

class BoolLiteralExpression : public Expression
{
public:
    BoolLiteralExpression(const Token* token);

    virtual ~BoolLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
};

class StringLiteralExpression : public Expression
{
public:
    StringLiteralExpression(const std::vector<char> characters, const Token* token);

    virtual ~StringLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
    std::vector<char> characters;
};

class ArraySizeValueExpression : public Expression
{
public:
    ArraySizeValueExpression(Expression* sizeExpression, Expression* valueExpression,
                             const Token* startToken, const Token* endToken);

    virtual ~ArraySizeValueExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* startToken;
    const Token* endToken;
    Expression* sizeExpression;
    Expression* valueExpression;
};

class ArrayMultiValueExpression : public Expression
{
public:
    ArrayMultiValueExpression(const Expressions& expressions,
                              const Token* startToken, const Token* endToken);

    virtual ~ArrayMultiValueExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* startToken;
    const Token* endToken;
    Expressions expressions;
};

class BlockExpression : public Expression
{
public:
    BlockExpression(const SyntaxTreeNodes& statements,
                    const Token* startToken, const Token* endToken);

    virtual ~BlockExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* startToken;
    const Token* endToken;
    SyntaxTreeNodes statements;
};

class BinaryExpression : public Expression
{
public:
    enum EOperator
    {
        eEqual                      = Token::eEqualEqual,
        eNotEqual                   = Token::eExclaimEqual,
        eLessThan                   = Token::eLess,
        eLessThanOrEqual            = Token::eLessEqual,
        eGreaterThan                = Token::eGreater,
        eGreaterThanOrEqual         = Token::eGreaterEqual,
        eAdd                        = Token::ePlus,
        eSubtract                   = Token::eMinus,
        eMultiply                   = Token::eTimes,
        eDivide                     = Token::eDivide,
        eRemainder                  = Token::eRemainder,
        eShiftLeft                  = Token::eLessLess,
        eShiftRightLogical          = Token::eGreaterGreater,
        eShiftRightArithmetic       = Token::eGreaterGreaterGreater,
        eBitwiseAnd                 = Token::eAmpersand,
        eBitwiseXor                 = Token::eCaret,
        eBitwiseOr                  = Token::eBar,
        eLogicalAnd                 = Token::eAmpersandAmpersand,
        eLogicalOr                  = Token::eBarBar,
        eAssign                     = Token::eEqual,
        eAddAssign                  = Token::ePlusEqual,
        eSubtractAssign             = Token::eMinusEqual,
        eMultiplyAssign             = Token::eTimesEqual,
        eDivideAssign               = Token::eDivideEqual,
        eRemainderAssign            = Token::eRemainderEqual,
        eShiftLeftAssign            = Token::eLessLessEqual,
        eShiftRightLogicalAssign    = Token::eGreaterGreaterEqual,
        eShiftRightArithmeticAssign = Token::eGreaterGreaterGreaterEqual,
        eBitwiseAndAssign           = Token::eAmpersandEqual,
        eBitwiseXorAssign           = Token::eCaretEqual,
        eBitwiseOrAssign            = Token::eBarEqual,
        eClosedRange                = Token::ePeriodPeriod,
        eHalfOpenRange              = Token::ePeriodPeriodLess,
        eSubscript                  = Token::eOpenBracket,
    };

    static const char* GetOperatorString(EOperator op);

    static unsigned GetPrecedence(EOperator op);

    static bool IsAssignment(EOperator op);

    static bool IsComputationAssignment(EOperator op);

    BinaryExpression(EOperator op, Expression* left, Expression* right, const Token* opToken, const Token* opToken2 = nullptr);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* opToken;
    const Token* opToken2;
    Expression* left;
    Expression* right;
    EOperator op;
};

class UnaryExpression : public Expression
{
public:
    enum EOperator
    {
        eNegative    = Token::eMinus,
        eComplement  = Token::eExclaim,
        eAddressOf   = Token::eAmpersand,
        eDereference = Token::eTimes,
        eArrayOf     = Token::eOpenBracket,
    };

    static const char* GetOperatorString(EOperator op);

    UnaryExpression(EOperator op, Expression* subExpr, const Token* opToken);

    virtual ~UnaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* opToken;
    Expression* subExpression;
    EOperator op;
};

class IdentifierExpression : public Expression
{
public:
    IdentifierExpression(ROString name, const Token* token);

    virtual ~IdentifierExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
    ROString name;
};

class CastExpression : public Expression
{
public:
    CastExpression(Expression* typeExpression, Expression* subExpression,
                   const Token* castToken,
                   const Token* openParToken,
                   const Token* closeParToken);

    virtual ~CastExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* typeExpression;
    Expression* subExpression;
    const Token* castToken;
    const Token* openParToken;
    const Token* closeParToken;
};

class ImplicitCastExpression : public Expression
{
public:
    ImplicitCastExpression(Expression* subExpression);

    virtual ~ImplicitCastExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* subExpression;
};

class FunctionCallExpression : public Expression
{
public:
    FunctionCallExpression(Expression* functionExpr, const Expressions& arguments,
                           const Token* openParToken, const Token* closeParToken);

    virtual ~FunctionCallExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* openParToken;
    const Token* closeParToken;
    const TypeInfo* functionType;
    Expression* functionExpression;
    Expressions arguments;
};

class MemberExpression : public Expression
{
public:
    MemberExpression(Expression* subExpr, ROString memberName,
                     const Token* opToken, const Token* memberNameToken);

    virtual ~MemberExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    void SetAccessType(EAccessType newAccessType) override;

    const Token* opToken;
    const Token* memberNameToken;
    Expression* subExpression;
    ROString memberName;
};

class BranchExpression : public Expression
{
public:
    BranchExpression(Expression* ifCondition,
                     Expression* ifExpression,
                     Expression* elseExpression,
                     const Token* ifToken,
                     const Token* elseToken);

    virtual ~BranchExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* ifToken;
    const Token* elseToken;

    Expression* ifCondition;
    Expression* ifExpression;
    Expression* elseExpression;
};

class ConstantDeclaration : public SyntaxTreeNode
{
public:
    ConstantDeclaration(ROString name, BinaryExpression* assignmentExpression,
                        Expression* typeExpression, const Token* nameToken);

    virtual ~ConstantDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    ROString name;
    const TypeInfo* constantType;
    BinaryExpression* assignmentExpression;
    Expression* typeExpression;
};

class VariableDeclaration : public SyntaxTreeNode
{
public:
    VariableDeclaration(ROString name, BinaryExpression* assignmentExpression,
                        Expression* typeExpression, const Token* nameToken);

    virtual ~VariableDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    ROString name;
    const TypeInfo* variableType;
    BinaryExpression* assignmentExpression;
    Expression* typeExpression;
};

typedef std::vector<VariableDeclaration*> VariableDeclarations;

class WhileLoop : public SyntaxTreeNode
{
public:
    WhileLoop(Expression* condition, BlockExpression* expression, const Token* whileToken);

    virtual ~WhileLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* condition;
    BlockExpression* expression;
    const Token* whileToken;
};

class ForLoop : public SyntaxTreeNode
{
public:
    ForLoop(ROString variableName,
            Expression* varTypeExpression,
            ROString indexName,
            Expression* indexTypeExpression,
            Expression* iterExpression,
            BlockExpression* expression, const Token* forToken,
            const Token* inToken, const Token* variableNameToken,
            const Token* indexNameToken);

    virtual ~ForLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    ROString variableName;
    Expression* varTypeExpression;
    const TypeInfo* variableType;
    ROString indexName;
    Expression* indexTypeExpression;
    const TypeInfo* indexType;
    Expression* iterExpression;
    BlockExpression* expression;
    const Token* forToken;
    const Token* inToken;
    const Token* variableNameToken;
    const Token* indexNameToken;
};

class LoopControl : public SyntaxTreeNode
{
public:
    LoopControl(const Token* token);

    virtual ~LoopControl() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
};

class Return : public SyntaxTreeNode
{
public:
    Return(const Token* token, Expression* expression);

    virtual ~Return();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
    Expression* expression;
};

class Parameter
{
public:
    Parameter(ROString name,
              Expression* typeExpression,
              const Token* nameToken);

    ~Parameter();

    const Token* nameToken;
    ROString name;
    Expression* typeExpression;
    const TypeInfo* type;
};

typedef std::vector<Parameter*> Parameters;

class FunctionDeclaration
{
public:
    FunctionDeclaration(ROString name,
                        const Parameters& parameters,
                        Expression* returnTypeExpression,
                        const Token* nameToken);

    virtual ~FunctionDeclaration();

    const Token* nameToken;
    ROString name;
    Parameters parameters;
    Expression* returnTypeExpression;
    const TypeInfo* returnType;
};

class FunctionTypeExpression : public Expression
{
public:
    FunctionTypeExpression(
        const Expressions& paramTypeExpressions,
        const std::vector<ROString>& paramNames,
        Expression* returnTypeExpression,
        const Token* funToken,
        const Token* openParToken,
        const Token* closeParToken,
        const std::vector<const Token*> paramNameTokens
    );

    virtual ~FunctionTypeExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* funToken;
    const Token* openParToken;
    const Token* closeParToken;
    std::vector<const Token*> paramNameTokens;
    Expressions paramTypeExpressions;
    std::vector<ROString> paramNames;
    Expression* returnTypeExpression;
};

class ExternFunctionDeclaration : public Expression
{
public:
    ExternFunctionDeclaration(FunctionDeclaration* declaration);

    virtual ~ExternFunctionDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    FunctionDeclaration* declaration;
};

class FunctionDefinition : public Expression
{
public:
    FunctionDefinition(FunctionDeclaration* declaration,
                       Expression* expression);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    FunctionDeclaration* declaration;
    Expression* expression;
    bool endsWithReturnStatement;
};

class MemberDefinition
{
public:
    MemberDefinition(ROString name, Expression* typeExpression,
                     const Token* nameToken);

    ~MemberDefinition();

    const Token* nameToken;
    Expression* typeExpression;
    ROString name;
};

class StructDefinition : public SyntaxTreeNode
{
public:
    StructDefinition(ROString name, const std::vector<MemberDefinition*>& members,
                     const Token* nameToken, unsigned fileId);

    virtual ~StructDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    ROString name;
    std::vector<MemberDefinition*> members;
    const TypeInfo* type;
    unsigned fileId;
};

class MemberInitialization
{
public:
    MemberInitialization(ROString name, Expression* expression,
                         const Token* nameToken);

    virtual ~MemberInitialization();

    ROString name;
    Expression* expression;
    const Token* nameToken;
};

class StructInitializationExpression : public Expression
{
public:
    StructInitializationExpression(ROString structName, const std::vector<MemberInitialization*>& memberInitializations,
                                   const Token* structNameToken,
                                   const Token* openBraceToken,
                                   const Token* closeBraceToken);

    virtual ~StructInitializationExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    ROString structName;
    std::vector<MemberInitialization*> memberInitializations;
    const Token* structNameToken;
    const Token* openBraceToken;
    const Token* closeBraceToken;
};

class ModuleDefinition : public SyntaxTreeNode
{
public:
    ModuleDefinition(unsigned fileId,
                     const std::vector<ConstantDeclaration*> constantDeclarations,
                     const std::vector<StructDefinition*>& structDefinitions,
                     const std::vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                     const std::vector<FunctionDefinition*>& functionDefinitions);

    virtual ~ModuleDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    unsigned fileId;
    std::vector<ConstantDeclaration*> constantDeclarations;
    std::vector<StructDefinition*> structDefinitions;
    std::vector<ExternFunctionDeclaration*> externFunctionDeclarations;
    std::vector<FunctionDefinition*> functionDefinitions;
};

class Modules : public SyntaxTreeNode
{
public:
    virtual ~Modules();

    void Accept(SyntaxTreeVisitor* visitor) override;

    std::vector<ModuleDefinition*> modules;
    std::vector<StructDefinition*> orderedStructDefinitions;
};
} // namespace SyntaxTree

namespace std
{
    template<>
    struct hash<SyntaxTree::BinaryExpression::EOperator>
    {
        std::size_t operator()(SyntaxTree::BinaryExpression::EOperator op) const
        {
            return static_cast<std::size_t>(op);
        }
    };
}

#endif // SYNTAX_TREE_H_
