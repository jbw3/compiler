#include "StartEndTokenFinder.h"
#include "SyntaxTree.h"
#include <cassert>

using namespace SyntaxTree;

void StartEndTokenFinder::Visit(UnaryExpression* unaryExpression)
{
    UpdateStart(unaryExpression->opToken);
    unaryExpression->subExpression->Accept(this);
}

void StartEndTokenFinder::Visit(BinaryExpression* binaryExpression)
{
    binaryExpression->left->Accept(this);

    if (binaryExpression->op == BinaryExpression::eSubscript)
    {
        UpdateEnd(binaryExpression->opToken2);
    }
    else
    {
        binaryExpression->right->Accept(this);
    }
}

void StartEndTokenFinder::Visit(WhileLoop* /*whileLoop*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(ForLoop* /*forLoop*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(LoopControl* /*loopControl*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(Return* /*ret*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(FunctionTypeExpression* functionTypeExpression)
{
    UpdateStart(functionTypeExpression->funToken);

    Expression* retTypeExpr = functionTypeExpression->returnTypeExpression;
    if (retTypeExpr == nullptr)
    {
        UpdateEnd(functionTypeExpression->closeParToken);
    }
    else
    {
        retTypeExpr->Accept(this);
    }
}

void StartEndTokenFinder::Visit(ExternFunctionDeclaration* /*externFunctionDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(FunctionDefinition* /*functionDefinition*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    UpdateStart(structDefinitionExpression->structToken);
    UpdateEnd(structDefinitionExpression->closeBraceToken);
}

void StartEndTokenFinder::Visit(StructInitializationExpression* structInitializationExpression)
{
    structInitializationExpression->structTypeExpression->Accept(this);
    UpdateEnd(structInitializationExpression->closeBraceToken);
}

void StartEndTokenFinder::Visit(ModuleDefinition* /*moduleDefinition*/)
{
    assert(false && "StartEndTokenFinder does not support ModuleDefinition");
}

void StartEndTokenFinder::Visit(Modules* /*modules*/)
{
    assert(false && "StartEndTokenFinder does not support Modules");
}

void StartEndTokenFinder::Visit(UnitTypeLiteralExpression* /*unitTypeLiteralExpression*/)
{
}

void StartEndTokenFinder::Visit(NumericExpression* numericExpression)
{
    const Token* token = numericExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    const Token* token = floatLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    const Token* token = boolLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(StringLiteralExpression* stringLiteralExpression)
{
    const Token* token = stringLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(IdentifierExpression* identifierExpression)
{
    const Token* token = identifierExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(BuiltInIdentifierExpression* builtInIdentifierExpression)
{
    const Token* token = builtInIdentifierExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(ArraySizeValueExpression* arrayExpression)
{
    UpdateStart(arrayExpression->startToken);
    UpdateEnd(arrayExpression->endToken);
}

void StartEndTokenFinder::Visit(ArrayMultiValueExpression* arrayExpression)
{
    UpdateStart(arrayExpression->startToken);
    UpdateEnd(arrayExpression->endToken);
}

void StartEndTokenFinder::Visit(BlockExpression* blockExpression)
{
    UpdateStart(blockExpression->startToken);
    UpdateEnd(blockExpression->endToken);
}

void StartEndTokenFinder::Visit(UncheckedBlock* /*uncheckedBlock*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(ImplicitCastExpression* castExpression)
{
    castExpression->subExpression->Accept(this);
}

void StartEndTokenFinder::Visit(FunctionCallExpression* functionCallExpression)
{
    functionCallExpression->functionExpression->Accept(this);
    UpdateEnd(functionCallExpression->closeParToken);
}

void StartEndTokenFinder::Visit(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    UpdateStart(builtInFunctionCallExpression->nameToken);
    UpdateEnd(builtInFunctionCallExpression->closeParToken);
}

void StartEndTokenFinder::Visit(MemberExpression* memberExpression)
{
    memberExpression->subExpression->Accept(this);
    UpdateEnd(memberExpression->memberNameToken);
}

void StartEndTokenFinder::Visit(BranchExpression* branchExpression)
{
    UpdateStart(branchExpression->ifToken);
    branchExpression->elseExpression->Accept(this);
}

void StartEndTokenFinder::Visit(ConstantDeclaration* /*constantDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(VariableDeclaration* /*variableDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::UpdateStart(const Token* newStart)
{
    if (start == nullptr || newStart->line < start->line || (newStart->line == start->line && newStart->column < start->column))
    {
        start = newStart;
    }
}

void StartEndTokenFinder::UpdateEnd(const Token* newEnd)
{
    if (end == nullptr || newEnd->line > end->line || (newEnd->line == end->line && newEnd->column > end->column))
    {
        end = newEnd;
    }
}
