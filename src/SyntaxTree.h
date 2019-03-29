#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <string>
#include <vector>

class SyntaxTreeVisitor;

namespace SyntaxTree
{
class SyntaxTreeNode
{
public:
    virtual ~SyntaxTreeNode() = default;

    virtual void Accept(SyntaxTreeVisitor* visitor) const = 0;
};

class Expression : public SyntaxTreeNode
{
public:
    virtual ~Expression() = default;
};

class NumericExpression : public Expression
{
public:
    NumericExpression(std::string number);

    virtual ~NumericExpression() = default;

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetNumber() const;

private:
    std::string number;
};

class BinaryExpression : public Expression
{
public:
    enum EOperator
    {
        eAdd,
        eSubtract,
    };

    BinaryExpression(EOperator op, Expression* left, Expression* right);

    virtual ~BinaryExpression();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    EOperator GetOperator() const;

    const Expression* GetLeftExpression() const;

    const Expression* GetRightExpression() const;

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

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetName() const;

private:
    std::string name;
};

class FunctionExpression : public Expression
{
public:
    FunctionExpression(const std::string& name, const std::vector<Expression*>& arguments);

    virtual ~FunctionExpression();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetName() const;

    const std::vector<Expression*>& GetArguments() const;

private:
    std::string name;
    std::vector<Expression*> arguments;
};

class VariableDefinition
{
public:
    VariableDefinition(const std::string& name);

    virtual ~VariableDefinition() = default;

    const std::string& GetName() const;

private:
    std::string name;
};

class FunctionDefinition : public SyntaxTreeNode
{
public:
    FunctionDefinition(const std::string& name, const std::vector<VariableDefinition*>& parameters,
                       SyntaxTreeNode* code);

    virtual ~FunctionDefinition();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::string& GetName() const;

    const std::vector<VariableDefinition*>& GetParameters() const;

    const SyntaxTreeNode* GetCode() const;

private:
    std::string name;
    std::vector<VariableDefinition*> parameters;
    SyntaxTreeNode* code;
};

class ModuleDefinition : public SyntaxTreeNode
{
public:
    ModuleDefinition(const std::vector<FunctionDefinition*>& functionDefinitions);

    virtual ~ModuleDefinition();

    void Accept(SyntaxTreeVisitor* visitor) const override;

    const std::vector<FunctionDefinition*>& GetFunctionDefinitions() const;

private:
    std::vector<FunctionDefinition*> functionDefinitions;
};
} // namespace SyntaxTree

#endif // SYNTAX_TREE_H_
