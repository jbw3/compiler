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
    Expression();

    virtual ~Expression() = default;

    const TypeInfo* GetType() const;

    void SetType(const TypeInfo* newType);

private:
    const TypeInfo* type;
};

typedef std::vector<Expression*> Expressions;

class UnitTypeLiteralExpression : public Expression
{
public:
    UnitTypeLiteralExpression();

    virtual ~UnitTypeLiteralExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;
};

class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetNumber() const;

    const TypeInfo* GetMinSizeType() const;

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

    const std::string& GetValue() const;

private:
    std::string value;
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
        eShiftRightArithmetic,
        eBitwiseAnd,
        eBitwiseXor,
        eBitwiseOr,
        eLogicalAnd,
        eLogicalOr,
    };

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

class VariableDefinition
{
public:
    VariableDefinition(const std::string& name, const TypeInfo* type);

    virtual ~VariableDefinition() = default;

    const std::string& GetName() const;

    const TypeInfo* GetType() const;

private:
    std::string name;
    const TypeInfo* type;
};

typedef std::vector<VariableDefinition*> VariableDefinitions;

class Assignment : public Expression
{
public:
    Assignment(const std::string& variableName, Expression* expression);

    virtual ~Assignment();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetVariableName() const;

    Expression* GetExpression() const;

private:
    std::string variableName;
    Expression* expression;
};

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

class FunctionDeclaration
{
public:
    FunctionDeclaration(const std::string& name,
                        const VariableDefinitions& parameters,
                        const TypeInfo* returnType);

    virtual ~FunctionDeclaration();

    const std::string& GetName() const;

    const VariableDefinitions& GetParameters() const;

    const TypeInfo* GetReturnType() const;

private:
    std::string name;
    VariableDefinitions parameters;
    const TypeInfo* returnType;
};

class FunctionDefinition : public SyntaxTreeNode
{
public:
    FunctionDefinition(FunctionDeclaration* declaration,
                       const VariableDefinitions& variableDefinitions,
                       Expression* expression);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const FunctionDeclaration* GetDeclaration() const;

    const VariableDefinitions& GetVariableDefinitions() const;

    Expression* GetExpression() const;

private:
    FunctionDeclaration* declaration;
    VariableDefinitions variableDefinitions;
    Expression* expression;
};

class ModuleDefinition : public SyntaxTreeNode
{
public:
    ModuleDefinition(const std::vector<FunctionDefinition*>& functionDefinitions);

    virtual ~ModuleDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::vector<FunctionDefinition*>& GetFunctionDefinitions() const;

private:
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
