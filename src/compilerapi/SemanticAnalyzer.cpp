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
    Expression* code = functionDefinition->GetCode();
    functionDefinition->GetCode()->Accept(this);
    if (isError)
    {
        return;
    }

    if (functionDefinition->GetReturnType() != code->GetType())
    {
        isError = true;
        cerr << "Function return expression does not equal return type\n";
    }
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    // build a look-up table for all functions
    functions.clear();
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        auto rv = functions.insert({funcDef->GetName(), funcDef});
        if (!rv.second)
        {
            isError = true;
            cerr << "Function \"" << funcDef->GetName() << "\" has already been defined\n";
            return;
        }
    }

    // perform semantic analysis on all functions
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
    const string& funcName = functionExpression->GetName();
    auto iter = functions.find(funcName);
    if (iter == functions.cend())
    {
        cerr << "Function \"" << funcName << "\" is not defined\n";
        isError = true;
    }
    else
    {
        functionExpression->SetType(iter->second->GetReturnType());
    }
}
