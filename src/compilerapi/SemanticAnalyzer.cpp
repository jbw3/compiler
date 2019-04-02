#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzer::SemanticAnalyzer() :
    isError(false)
{
}

bool SemanticAnalyzer::Process(SyntaxTreeNode* syntaxTree)
{
    syntaxTree->Accept(this);
    return !isError;
}

void SemanticAnalyzer::Visit(BinaryExpression* binaryExpression)
{
    Expression* left = binaryExpression->GetLeftExpression();
    Expression* right = binaryExpression->GetRightExpression();

    left->Accept(this);
    if (isError)
    {
        return;
    }

    right->Accept(this);
    if (isError)
    {
        return;
    }

    if (left->GetType() != right->GetType())
    {
        cerr << "Left and right operands do not have the same type\n";
        isError = true;
    }
    else
    {
        binaryExpression->SetType(left->GetType());
    }
}

void SemanticAnalyzer::Visit(FunctionDefinition* functionDefinition)
{
    functionDefinition->GetCode()->Accept(this);
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        funcDef->Accept(this);
        if (isError)
        {
            break;
        }
    }
}

void SemanticAnalyzer::Visit(NumericExpression* numericExpression)
{
    // TODO: check number's type
    numericExpression->SetType(Expression::eInt32);
}

void SemanticAnalyzer::Visit(VariableExpression* variableExpression)
{
    // TODO: look up type in scope
    variableExpression->SetType(Expression::eInt32);
}

void SemanticAnalyzer::Visit(FunctionExpression* functionExpression)
{
    // TODO: look up type in scope
    functionExpression->SetType(Expression::eInt32);
}
