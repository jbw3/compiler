#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <functional>
#include <string>
#include <vector>
#include "TypeInfo.h"

class SyntaxTreeVisitor;

namespace SyntaxTree
{
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

    virtual bool GetIsLiteral() const;

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

    bool GetIsLiteral() const override;
};

// TODO: Rename to NumericLiteralExpression
class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    bool GetIsLiteral() const override;

    const std::string& GetNumber() const;

    unsigned GetMinSignedSize() const;

    unsigned GetMinUnsignedSize() const;

private:
    std::string number;
};

class BoolLiteralExpression : public Expression
{
public:
    static BoolLiteralExpression* CreateTrueExpression();

    static BoolLiteralExpression* CreateFalseExpression();

    BoolLiteralExpression(const std::string& value);

    virtual ~BoolLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    bool GetIsLiteral() const override;

    const std::string& GetValue() const;

private:
    std::string value;
};

class StringLiteralExpression : public Expression
{
public:
    StringLiteralExpression(const std::vector<char> characters);

    virtual ~StringLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    bool GetIsLiteral() const override;

    const std::vector<char>& GetCharacters() const;

private:
    std::vector<char> characters;
};

class BlockExpression : public Expression
{
public:
    BlockExpression(const Expressions& expressions);

    virtual ~BlockExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const Expressions& GetExpressions() const;

private:
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
    };

    static bool IsAssignment(EOperator op);

    static bool IsComputationAssignment(EOperator op);

    BinaryExpression(EOperator op, Expression* left, Expression* right);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    EOperator GetOperator() const;

    Expression* GetLeftExpression() const;

    Expression* GetRightExpression() const;

private:
    EOperator op;
    Expression* left;
    Expression* right;
};

class UnaryExpression : public Expression
{
public:
    enum EOperator
    {
        eNegative,
        eComplement,
    };

    UnaryExpression(EOperator op, Expression* subExpr);

    virtual ~UnaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    EOperator GetOperator() const;

    Expression* GetSubExpression() const;

private:
    EOperator op;
    Expression* subExpression;
};

class VariableExpression : public Expression
{
public:
    VariableExpression(const std::string& name);

    virtual ~VariableExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

private:
    std::string name;
};

class FunctionExpression : public Expression
{
public:
    FunctionExpression(const std::string& name, const Expressions& arguments);

    virtual ~FunctionExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const Expressions& GetArguments() const;

private:
    std::string name;
    Expressions arguments;
};

class MemberExpression : public Expression
{
public:
    MemberExpression(Expression* subExpr, const std::string& memberName);

    virtual ~MemberExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    void SetAccessType(EAccessType newAccessType) override;

    Expression* GetSubExpression() const;

    const std::string& GetMemberName() const;

private:
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
    VariableDeclaration(const std::string& name, const std::string& typeName, BinaryExpression* assignmentExpression);

    virtual ~VariableDeclaration() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const std::string& GetTypeName() const;

    const TypeInfo* GetVariableType() const;

    void SetVariableType(const TypeInfo* newType);

    BinaryExpression* GetAssignmentExpression() const;

private:
    std::string name;
    std::string typeName;
    const TypeInfo* variableType;
    BinaryExpression* assignmentExpression;
};

typedef std::vector<VariableDeclaration*> VariableDeclarations;

class WhileLoop : public Expression
{
public:
    WhileLoop(Expression* condition, Expression* expression);

    virtual ~WhileLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* GetCondition() const;

    Expression* GetExpression() const;

private:
    Expression* condition;
    Expression* expression;
};

class Parameter
{
public:
    Parameter(const std::string& name, const std::string& typeName);

    ~Parameter() = default;

    const std::string& GetName() const;

    const std::string& GetTypeName() const;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

private:
    std::string name;
    std::string typeName;
    const TypeInfo* type;
};

typedef std::vector<Parameter*> Parameters;

class FunctionDeclaration
{
public:
    FunctionDeclaration(const std::string& name,
                        const Parameters& parameters,
                        const std::string& returnTypeName);

    virtual ~FunctionDeclaration();

    const std::string& GetName() const;

    const Parameters& GetParameters() const;

    const std::string& GetReturnTypeName() const;

    const TypeInfo* GetReturnType() const;

    void SetReturnType(const TypeInfo* newType);

private:
    std::string name;
    Parameters parameters;
    std::string returnTypeName;
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
    MemberDefinition(const std::string& name, const std::string& typeName);

    const std::string& GetName() const;

    const std::string& GetTypeName() const;

private:
    std::string name;
    std::string typeName;
};

class StructDefinition : public SyntaxTreeNode
{
public:
    StructDefinition(const std::string& name, const std::vector<MemberDefinition*>& members);

    virtual ~StructDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const std::vector<MemberDefinition*>& GetMembers() const;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

private:
    std::string name;
    std::vector<MemberDefinition*> members;
    const TypeInfo* type;
};

class MemberInitialization
{
public:
    MemberInitialization(const std::string& name, Expression* expression);

    virtual ~MemberInitialization();

    const std::string& GetName() const;

    Expression* GetExpression() const;

private:
    std::string name;
    Expression* expression;
};

class StructInitializationExpression : public Expression
{
public:
    StructInitializationExpression(const std::string& structName, const std::vector<MemberInitialization*>& memberInitializations);

    virtual ~StructInitializationExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetStructName() const;

    const std::vector<MemberInitialization*>& GetMemberInitializations() const;

private:
    std::string structName;
    std::vector<MemberInitialization*> memberInitializations;
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
