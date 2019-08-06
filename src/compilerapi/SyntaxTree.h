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

typedef std::vector<SyntaxTreeNode*> Statements;

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
        eModulo,
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
    FunctionExpression(const std::string& name, const std::vector<Expression*>& arguments);

    virtual ~FunctionExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const std::vector<Expression*>& GetArguments() const;

private:
    std::string name;
    std::vector<Expression*> arguments;
};

class BranchExpression : public Expression
{
public:
    BranchExpression(Expression* ifCondition,
                     const Statements& ifStatements,
                     Expression* ifExpression,
                     const Statements& elseStatements,
                     Expression* elseExpression);

    virtual ~BranchExpression();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* GetIfCondition() const;

    const Statements& GetIfStatements() const;

    Expression* GetIfExpression() const;

    const Statements& GetElseStatements() const;

    Expression* GetElseExpression() const;

private:
    Expression* ifCondition;
    Statements ifStatements;
    Expression* ifExpression;
    Statements elseStatements;
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

class Assignment : public SyntaxTreeNode
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

class WhileLoop : public SyntaxTreeNode
{
public:
    WhileLoop(Expression* condition, const Statements& statements);

    virtual ~WhileLoop();

    void Accept(SyntaxTreeVisitor* visitor) override;

    Expression* GetCondition() const;

    const Statements& GetStatements() const;

private:
    Expression* condition;
    Statements statements;
};

class FunctionDefinition : public SyntaxTreeNode
{
public:
    FunctionDefinition(const std::string& name, const VariableDefinitions& parameters,
                       const TypeInfo* returnType, const VariableDefinitions& variableDefinitions,
                       const Statements& statements, Expression* returnExpression);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const VariableDefinitions& GetParameters() const;

    const TypeInfo* GetReturnType() const;

    const VariableDefinitions& GetVariableDefinitions() const;

    const Statements& GetStatements() const;

    Expression* GetReturnExpression() const;

private:
    std::string name;
    VariableDefinitions parameters;
    const TypeInfo* returnType;
    VariableDefinitions variableDefinitions;
    Statements statements;
    Expression* returnExpression;
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
