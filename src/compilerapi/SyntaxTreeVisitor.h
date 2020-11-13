#ifndef SYNTAX_TREE_VISITOR_H_
#define SYNTAX_TREE_VISITOR_H_

namespace SyntaxTree
{
class ArrayMultiValueExpression;
class ArraySizeValueExpression;
class BinaryExpression;
class BoolLiteralExpression;
class BlockExpression;
class BranchExpression;
class ExternFunctionDeclaration;
class ForLoop;
class FunctionDefinition;
class FunctionExpression;
class LoopControl;
class MemberExpression;
class ModuleDefinition;
class NumericExpression;
class Return;
class StringLiteralExpression;
class StructDefinition;
class StructInitializationExpression;
class UnaryExpression;
class UnitTypeLiteralExpression;
class VariableDeclaration;
class VariableExpression;
class WhileLoop;
} // namespace SyntaxTree

class SyntaxTreeVisitor
{
public:
    virtual void Visit(SyntaxTree::UnaryExpression* unaryExpression) = 0;

    virtual void Visit(SyntaxTree::BinaryExpression* binaryExpression) = 0;

    virtual void Visit(SyntaxTree::WhileLoop* whileLoop) = 0;

    virtual void Visit(SyntaxTree::ForLoop* forLoop) = 0;

    virtual void Visit(SyntaxTree::LoopControl* loopControl) = 0;

    virtual void Visit(SyntaxTree::Return* ret) = 0;

    virtual void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) = 0;

    virtual void Visit(SyntaxTree::FunctionDefinition* functionDefinition) = 0;

    virtual void Visit(SyntaxTree::StructDefinition* structDefinition) = 0;

    virtual void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) = 0;

    virtual void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) = 0;

    virtual void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::VariableExpression* variableExpression) = 0;

    virtual void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) = 0;

    virtual void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) = 0;

    virtual void Visit(SyntaxTree::BlockExpression* blockExpression) = 0;

    virtual void Visit(SyntaxTree::FunctionExpression* functionExpression) = 0;

    virtual void Visit(SyntaxTree::MemberExpression* memberExpression) = 0;

    virtual void Visit(SyntaxTree::BranchExpression* branchExpression) = 0;

    virtual void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
