#include "SyntaxTreeVisitor.h"

class Token;

class StartEndTokenFinder : public SyntaxTreeVisitor
{
public:
    const Token* start = nullptr;
    const Token* end = nullptr;

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ForLoop* forLoop) override;

    void Visit(SyntaxTree::LoopControl* loopControl) override;

    void Visit(SyntaxTree::Return* ret) override;

    void Visit(SyntaxTree::FunctionTypeExpression* functionTypeExpression) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::StructDefinitionExpression* structDefinitionExpression) override;

    void Visit(SyntaxTree::StructInitializationExpression* structInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::Modules* modules) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::FloatLiteralExpression* floatLiteralExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::IdentifierExpression* identifierExpression) override;

    void Visit(SyntaxTree::BuiltInIdentifierExpression* builtInIdentifierExpression) override;

    void Visit(SyntaxTree::ArraySizeValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::ArrayMultiValueExpression* arrayExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::UncheckedBlock* uncheckedBlock) override;

    void Visit(SyntaxTree::ImplicitCastExpression* castExpression) override;

    void Visit(SyntaxTree::FunctionCallExpression* functionCallExpression) override;

    void Visit(SyntaxTree::BuiltInFunctionCallExpression* builtInFunctionCallExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::ConstantDeclaration* constantDeclaration) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

private:
    void UpdateStart(const Token* newStart);

    void UpdateEnd(const Token* newEnd);
};
