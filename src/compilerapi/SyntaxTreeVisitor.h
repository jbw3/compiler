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
class BuiltInFunctionCallExpression;
class BuiltInIdentifierExpression;
class ConstantDeclaration;
class ExternFunctionDeclaration;
class FloatLiteralExpression;
class ForLoop;
class FunctionDefinition;
class FunctionCallExpression;
class FunctionTypeExpression;
class IdentifierExpression;
class ImplicitCastExpression;
class LoopControl;
class MemberExpression;
class ModuleDefinition;
class Modules;
class NumericExpression;
class Return;
class StringLiteralExpression;
class StructDefinitionExpression;
class StructInitializationExpression;
class UnaryExpression;
class UncheckedBlock;
class UnitTypeLiteralExpression;
class VariableDeclaration;
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

    virtual void Visit(SyntaxTree::FunctionTypeExpression* functionTypeExpression) = 0;

    virtual void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) = 0;

    virtual void Visit(SyntaxTree::FunctionDefinition* functionDefinition) = 0;

    virtual void Visit(SyntaxTree::StructDefinitionExpression* structDefinitionExpression) = 0;

    virtual void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) = 0;

    virtual void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) = 0;

    virtual void Visit(SyntaxTree::Modules* modules) = 0;

    virtual void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(SyntaxTree::FloatLiteralExpression* floatLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) = 0;

    virtual void Visit(SyntaxTree::IdentifierExpression* identifierExpression) = 0;

    virtual void Visit(SyntaxTree::BuiltInIdentifierExpression* builtInIdentifierExpression) = 0;

    virtual void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) = 0;

    virtual void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) = 0;

    virtual void Visit(SyntaxTree::BlockExpression* blockExpression) = 0;

    virtual void Visit(SyntaxTree::UncheckedBlock* uncheckedBlock) = 0;

    virtual void Visit(SyntaxTree::ImplicitCastExpression* castExpression) = 0;

    virtual void Visit(SyntaxTree::FunctionCallExpression* functionCallExpression) = 0;

    virtual void Visit(SyntaxTree::BuiltInFunctionCallExpression* builtInFunctionCallExpression) = 0;

    virtual void Visit(SyntaxTree::MemberExpression* memberExpression) = 0;

    virtual void Visit(SyntaxTree::BranchExpression* branchExpression) = 0;

    virtual void Visit(SyntaxTree::ConstantDeclaration* constantDeclaration) = 0;

    virtual void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
