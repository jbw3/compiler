#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <functional>
#include <string>
#include <vector>
#include "TypeInfo.h"

class Token;
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

class Expression : public SyntaxTreeNode
{
public:
    enum EAccessType
    {
        eLoad,
        eStore,
    };

    Expression();

    virtual ~Expression() = default;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

    bool GetIsAssignable() const;

    void SetIsAssignable(bool newIsAssignable);

    EAccessType GetAccessType() const;

    virtual void SetAccessType(EAccessType newAccessType);

private:
    const TypeInfo* type;
    bool isAssignable;
    EAccessType accessType;
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

    int64_t GetValue() const;

    const Token* GetToken() const;

    unsigned GetMinSignedSize() const;

    unsigned GetMinUnsignedSize() const;

private:
    const Token* token;
    int64_t value;
};

class BoolLiteralExpression : public Expression
{
public:
    BoolLiteralExpression(bool value, const Token* token);

    virtual ~BoolLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    bool GetValue() const;

    const Token* GetToken() const;

private:
    const Token* token;
    bool value;
};

class StringLiteralExpression : public Expression
{
public:
    StringLiteralExpression(const std::vector<char> characters, const Token* token);

    virtual ~StringLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::vector<char>& GetCharacters() const;

    const Token* GetToken() const;

private:
    const Token* token;
    std::vector<char> characters;
};

class BlockExpression : public Expression
{
public:
    BlockExpression(const Expressions& expressions,
                    const Token* startToken, const Token* endToken);

    virtual ~BlockExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Expressions& GetExpressions() const;

    const Token* GetStartToken() const;

    const Token* GetEndToken() const;

private:
    const Token* startToken;
    const Token* endToken;
    Expressions expressions;
};

class BinaryExpression : public Expression
{
public:
    enum EOperator
    {
        eEqual,
        eNotEqual,
        eLessThan,
        eLessThanOrEqual,
        eGreaterThan,
        eGreaterThanOrEqual,
        eAdd,
        eSubtract,
        eMultiply,
        eDivide,
        eRemainder,
        eShiftLeft,
        eShiftRightLogical,
        eShiftRightArithmetic,
        eBitwiseAnd,
        eBitwiseXor,
        eBitwiseOr,
        eLogicalAnd,
        eLogicalOr,
        eAssign,
        eAddAssign,
        eSubtractAssign,
        eMultiplyAssign,
        eDivideAssign,
        eRemainderAssign,
        eShiftLeftAssign,
        eShiftRightLogicalAssign,
        eShiftRightArithmeticAssign,
        eBitwiseAndAssign,
        eBitwiseXorAssign,
        eBitwiseOrAssign,
        eInclusiveRange,
        eExclusiveRange,
    };

    static std::string GetOperatorString(EOperator op);

    static bool IsAssignment(EOperator op);

    static bool IsComputationAssignment(EOperator op);

    BinaryExpression(EOperator op, Expression* left, Expression* right, const Token* opToken);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    EOperator GetOperator() const;

    Expression* GetLeftExpression() const;

    Expression* GetRightExpression() const;

    const Token* GetOperatorToken() const;

private:
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
        eNegative,
        eComplement,
        eAddressOf,
        eDereference,
    };

    static std::string GetOperatorString(EOperator op);

    UnaryExpression(EOperator op, Expression* subExpr, const Token* opToken);

    virtual ~UnaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    EOperator GetOperator() const;

    Expression* GetSubExpression() const;

    const Token* GetOperatorToken() const;

private:
    const Token* opToken;
    Expression* subExpression;
    EOperator op;
};

class VariableExpression : public Expression
{
public:
    VariableExpression(const std::string& name, const Token* token);

    virtual ~VariableExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const Token* GetToken() const;

private:
    const Token* token;
    std::string name;
};

class FunctionExpression : public Expression
{
public:
    FunctionExpression(const std::string& name, const Expressions& arguments, const Token* nameToken);

    virtual ~FunctionExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const Expressions& GetArguments() const;

    const Token* GetNameToken() const;

    const FunctionDeclaration* GetFunctionDeclaration() const;

    void SetFunctionDeclaration(const FunctionDeclaration* decl);

private:
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

    Expression* GetSubExpression() const;

    const std::string& GetMemberName() const;

    const Token* GetOperatorToken() const;

    const Token* GetMemberNameToken() const;

private:
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
                     Expression* elseExpression);

    virtual ~BranchExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* GetIfCondition() const;

    Expression* GetIfExpression() const;

    Expression* GetElseExpression() const;

private:
    Expression* ifCondition;
    Expression* ifExpression;
    Expression* elseExpression;
};

class VariableDeclaration : public Expression
{
public:
    VariableDeclaration(const std::string& name, BinaryExpression* assignmentExpression,
                        const Token* nameToken, const std::vector<const Token*>& typeNameTokens);

    virtual ~VariableDeclaration() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const TypeInfo* GetVariableType() const;

    void SetVariableType(const TypeInfo* newType);

    BinaryExpression* GetAssignmentExpression() const;

    const Token* GetNameToken() const;

    const std::vector<const Token*>& GetTypeNameTokens() const;

private:
    const Token* nameToken;
    std::vector<const Token*> typeNameTokens;
    std::string name;
    const TypeInfo* variableType;
    BinaryExpression* assignmentExpression;
};

typedef std::vector<VariableDeclaration*> VariableDeclarations;

class WhileLoop : public Expression
{
public:
    WhileLoop(Expression* condition, BlockExpression* expression, const Token* whileToken);

    virtual ~WhileLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* GetCondition() const;

    BlockExpression* GetExpression() const;

    const Token* GetWhileToken() const;

private:
    Expression* condition;
    BlockExpression* expression;
    const Token* whileToken;
};

class ForLoop : public Expression
{
public:
    ForLoop(const std::string& variableName, Expression* iterExpression,
            BlockExpression* expression, const Token* forToken,
            const Token* inToken, const Token* variableNameToken,
            const std::vector<const Token*>& variableTypeNameTokens);

    virtual ~ForLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetVariableName() const;

    const TypeInfo* GetVariableType() const;

    void SetVariableType(const TypeInfo* newType);

    Expression* GetIterExpression();

    BlockExpression* GetExpression();

    const Token* GetForToken() const;

    const Token* GetInToken() const;

    const Token* GetVariableNameToken() const;

    const std::vector<const Token*>& GetVariableTypeNameTokens() const;

private:
    std::string variableName;
    const TypeInfo* variableType;
    Expression* iterExpression;
    BlockExpression* expression;
    const Token* forToken;
    const Token* inToken;
    const Token* variableNameToken;
    std::vector<const Token*> variableTypeNameTokens;
};

class Parameter
{
public:
    Parameter(const std::string& name, const Token* nameToken,
              const std::vector<const Token*>& typeNameTokens);

    ~Parameter() = default;

    const std::string& GetName() const;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

    const Token* GetNameToken() const;

    const std::vector<const Token*>& GetTypeNameTokens() const;

private:
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

    const std::string& GetName() const;

    const Parameters& GetParameters() const;

    const TypeInfo* GetReturnType() const;

    void SetReturnType(const TypeInfo* newType);

    const Token* GetNameToken() const;

    const std::vector<const Token*>& GetReturnTypeNameTokens() const;

private:
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

    FunctionDeclaration* GetDeclaration() const;

private:
    FunctionDeclaration* declaration;
};

class FunctionDefinition : public SyntaxTreeNode
{
public:
    FunctionDefinition(FunctionDeclaration* declaration,
                       Expression* expression);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    FunctionDeclaration* GetDeclaration() const;

    Expression* GetExpression() const;

private:
    FunctionDeclaration* declaration;
    Expression* expression;
};

class MemberDefinition
{
public:
    MemberDefinition(const std::string& name, const Token* nameToken,
                     const std::vector<const Token*>& typeNameTokens);

    const std::string& GetName() const;

    const Token* GetNameToken() const;

    const std::vector<const Token*>& GetTypeNameTokens() const;

private:
    const Token* nameToken;
    std::vector<const Token*> typeNameTokens;
    std::string name;
};

class StructDefinition : public SyntaxTreeNode
{
public:
    StructDefinition(const std::string& name, const std::vector<MemberDefinition*>& members,
                     const Token* nameToken);

    virtual ~StructDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const std::vector<MemberDefinition*>& GetMembers() const;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

    const Token* GetNameToken() const;

private:
    const Token* nameToken;
    std::string name;
    std::vector<MemberDefinition*> members;
    const TypeInfo* type;
};

class MemberInitialization
{
public:
    MemberInitialization(const std::string& name, Expression* expression,
                         const Token* nameToken);

    virtual ~MemberInitialization();

    const std::string& GetName() const;

    Expression* GetExpression() const;

    const Token* GetNameToken() const;

private:
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

    const std::string& GetStructName() const;

    const std::vector<MemberInitialization*>& GetMemberInitializations() const;

    const Token* GetStructNameToken() const;

private:
    std::string structName;
    std::vector<MemberInitialization*> memberInitializations;
    const Token* structNameToken;
};

class ModuleDefinition : public SyntaxTreeNode
{
public:
    ModuleDefinition(const std::vector<StructDefinition*>& structDefinitions,
                     const std::vector<ExternFunctionDeclaration*>& externFunctionDeclarations,
                     const std::vector<FunctionDefinition*>& functionDefinitions);

    virtual ~ModuleDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    void SwapStructDefinitions(std::vector<StructDefinition*>& newStructDefinitions);

    const std::vector<StructDefinition*>& GetStructDefinitions() const;

    const std::vector<ExternFunctionDeclaration*>& GetExternFunctionDeclarations() const;

    const std::vector<FunctionDefinition*>& GetFunctionDefinitions() const;

private:
    std::vector<StructDefinition*> structDefinitions;
    std::vector<ExternFunctionDeclaration*> externFunctionDeclarations;
    std::vector<FunctionDefinition*> functionDefinitions;
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
