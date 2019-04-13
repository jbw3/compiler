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
        return;
    }

    if (!CheckBinaryOperatorType(binaryExpression->GetOperator(), left->GetType()))
    {
        cerr << "Binary operator does not support type\n";
        isError = true;
        return;
    }

    binaryExpression->SetType(left->GetType());
}

bool SemanticAnalyzer::CheckBinaryOperatorType(BinaryExpression::EOperator op, EType type)
{
    switch (op)
    {
        case BinaryExpression::eAdd:
            return type == EType::eInt32;
        case BinaryExpression::eSubtract:
            return type == EType::eInt32;
        case BinaryExpression::eMultiply:
            return type == EType::eInt32;
        case BinaryExpression::eDivide:
            return type == EType::eInt32;
        case BinaryExpression::eModulo:
            return type == EType::eInt32;
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
        variables.clear();
        for (VariableDefinition* varDef : funcDef->GetParameters())
        {
            auto rv = variables.insert({varDef->GetName(), varDef});
            if (!rv.second)
            {
                isError = true;
                cerr << "Variable \"" << varDef->GetName() << "\" has already been defined\n";
                return;
            }
        }

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
    numericExpression->SetType(EType::eInt32);
}

void SemanticAnalyzer::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    boolLiteralExpression->SetType(EType::eBool);
}

void SemanticAnalyzer::Visit(VariableExpression* variableExpression)
{
    const string& varName = variableExpression->GetName();
    auto iter = variables.find(varName);
    if (iter == variables.cend())
    {
        cerr << "Variable \"" << varName << "\" is not defined\n";
        isError = true;
    }
    else
    {
        variableExpression->SetType(iter->second->GetType());
    }
}

void SemanticAnalyzer::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    auto iter = functions.find(funcName);
    if (iter == functions.cend())
    {
        cerr << "Function \"" << funcName << "\" is not defined\n";
        isError = true;
        return;
    }

    const FunctionDefinition* funcDef = iter->second;

    // check argument count
    const vector<Expression*>& args = functionExpression->GetArguments();
    const vector<VariableDefinition*>& params = funcDef->GetParameters();
    if (args.size() != params.size())
    {
        cerr << "Function '" << funcName << "' expected " << params.size() << " arguments but got " << args.size() << "\n";
        isError = true;
        return;
    }

    // process arguments
    for (size_t i = 0; i < args.size(); ++i)
    {
        // set argument type
        Expression* arg = args[i];
        arg->Accept(this);
        if (isError)
        {
            return;
        }

        // check argument against the parameter type
        const VariableDefinition* param = params[i];
        if (arg->GetType() != param->GetType())
        {
            cerr << "Argument does not match parameter type\n";
            isError = true;
            return;
        }
    }

    // set expression's type to the function's return type
    functionExpression->SetType(funcDef->GetReturnType());
}
