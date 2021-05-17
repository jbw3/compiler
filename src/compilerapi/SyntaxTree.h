#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <functional>
#include <string>
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

    static std::string GetOperatorString(EOperator op);

    static unsigned GetPrecedence(EOperator op);

    static bool IsAssignment(EOperator op);

    static bool IsComputationAssignment(EOperator op);

    BinaryExpression(EOperator op, Expression* left, Expression* right, const Token* opToken);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* opToken;
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
    };

    static std::string GetOperatorString(EOperator op);

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
    IdentifierExpression(const std::string& name, const Token* token);

    virtual ~IdentifierExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* token;
    std::string name;
};

class CastExpression : public Expression
{
public:
    CastExpression(Expression* subExpression, const Token* castToken,
                   const std::vector<const Token*>& castTypeNameTokens);

    virtual ~CastExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* subExpression;
    const Token* castToken;
    std::vector<const Token*> castTypeNameTokens;
};

class ImplicitCastExpression : public Expression
{
public:
    ImplicitCastExpression(Expression* subExpression);

    virtual ~ImplicitCastExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* subExpression;
};

class FunctionExpression : public Expression
{
public:
    FunctionExpression(const std::string& name, const Expressions& arguments, const Token* nameToken);

    virtual ~FunctionExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    const FunctionDeclaration* functionDeclaration;
    std::string name;
    Expressions arguments;
};

class MemberExpression : public Expression
{
public:
    MemberExpression(Expression* subExpr, const std::string& memberName,
                     const Token* opToken, const Token* memberNameToken);

    virtual ~MemberExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    void SetAccessType(EAccessType newAccessType) override;

    const Token* opToken;
    const Token* memberNameToken;
    Expression* subExpression;
    std::string memberName;
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
    ConstantDeclaration(const std::string& name, BinaryExpression* assignmentExpression,
                        const Token* nameToken, const std::vector<const Token*>& typeNameTokens);

    virtual ~ConstantDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    std::vector<const Token*> typeNameTokens;
    std::string name;
    const TypeInfo* constantType;
    BinaryExpression* assignmentExpression;
};

class VariableDeclaration : public SyntaxTreeNode
{
public:
    VariableDeclaration(const std::string& name, BinaryExpression* assignmentExpression,
                        const Token* nameToken, const std::vector<const Token*>& typeNameTokens);

    virtual ~VariableDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    std::vector<const Token*> typeNameTokens;
    std::string name;
    const TypeInfo* variableType;
    BinaryExpression* assignmentExpression;
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
    ForLoop(const std::string& variableName, const std::string& indexName,
            Expression* iterExpression,
            BlockExpression* expression, const Token* forToken,
            const Token* inToken, const Token* variableNameToken,
            const std::vector<const Token*>& variableTypeNameTokens,
            const Token* indexNameToken,
            const std::vector<const Token*>& indexTypeNameTokens);

    virtual ~ForLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    std::string variableName;
    const TypeInfo* variableType;
    std::string indexName;
    const TypeInfo* indexType;
    Expression* iterExpression;
    BlockExpression* expression;
    const Token* forToken;
    const Token* inToken;
    const Token* variableNameToken;
    std::vector<const Token*> variableTypeNameTokens;
    const Token* indexNameToken;
    std::vector<const Token*> indexTypeNameTokens;
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
    Parameter(const std::string& name, const Token* nameToken,
              const std::vector<const Token*>& typeNameTokens);

    ~Parameter() = default;

    const Token* nameToken;
    const std::vector<const Token*> typeNameTokens;
    std::string name;
    const TypeInfo* type;
};

typedef std::vector<Parameter*> Parameters;

class FunctionDeclaration
{
public:
    FunctionDeclaration(const std::string& name,
                        const Parameters& parameters,
                        const Token* nameToken,
                        const std::vector<const Token*>& returnTypeNameTokens);

    virtual ~FunctionDeclaration();

    const Token* nameToken;
    std::vector<const Token*> returnTypeNameTokens;
    std::string name;
    Parameters parameters;
    const TypeInfo* returnType;
};

class ExternFunctionDeclaration : public SyntaxTreeNode
{
public:
    ExternFunctionDeclaration(FunctionDeclaration* declaration);

    virtual ~ExternFunctionDeclaration();

    void Accept(SyntaxTreeVisitor* visitor) override;

    FunctionDeclaration* declaration;
};

class FunctionDefinition : public SyntaxTreeNode
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
    MemberDefinition(const std::string& name, const Token* nameToken,
                     const std::vector<const Token*>& typeNameTokens);

    const Token* nameToken;
    std::vector<const Token*> typeNameTokens;
    std::string name;
};

class StructDefinition : public SyntaxTreeNode
{
public:
    StructDefinition(const std::string& name, const std::vector<MemberDefinition*>& members,
                     const Token* nameToken, unsigned fileId);

    virtual ~StructDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Token* nameToken;
    std::string name;
    std::vector<MemberDefinition*> members;
    const TypeInfo* type;
    unsigned fileId;
};

class MemberInitialization
{
public:
    MemberInitialization(const std::string& name, Expression* expression,
                         const Token* nameToken);

    virtual ~MemberInitialization();

    std::string name;
    Expression* expression;
    const Token* nameToken;
};

class StructInitializationExpression : public Expression
{
public:
    StructInitializationExpression(const std::string& structName, const std::vector<MemberInitialization*>& memberInitializations,
                                   const Token* structNameToken);

    virtual ~StructInitializationExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    std::string structName;
    std::vector<MemberInitialization*> memberInitializations;
    const Token* structNameToken;
};

class ModuleDefinition : public SyntaxTreeNode
{
public:
    ModuleDefinition(unsigned fileId,
                     const std::vector<StructDefinition*>& structDefinitions,
                     const std::vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                     const std::vector<FunctionDefinition*>& functionDefinitions);

    virtual ~ModuleDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    unsigned fileId;
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
