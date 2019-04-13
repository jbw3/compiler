#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <functional>
#include <string>
#include <vector>

class SyntaxTreeVisitor;

namespace SyntaxTree
{
enum class EType
{
    eUnknown,
    eBool,
    eInt32,
};

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

    EType GetType() const;

    void SetType(EType newType);

private:
    EType type;
};

class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetNumber() const;

private:
    std::string number;
};

class BoolLiteralExpression : public Expression
{
public:
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
        eAdd,
        eSubtract,
        eMultiply,
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

class VariableDefinition
{
public:
    VariableDefinition(const std::string& name, EType type);

    virtual ~VariableDefinition() = default;

    const std::string& GetName() const;

    EType GetType() const;

private:
    std::string name;
    EType type;
};

class FunctionDefinition : public SyntaxTreeNode
{
public:
    FunctionDefinition(const std::string& name, const std::vector<VariableDefinition*>& parameters,
                       EType returnType, Expression* code);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) override;

    const std::string& GetName() const;

    const std::vector<VariableDefinition*>& GetParameters() const;

    EType GetReturnType() const;

    Expression* GetCode() const;

private:
    std::string name;
    std::vector<VariableDefinition*> parameters;
    EType returnType;
    Expression* code;
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
