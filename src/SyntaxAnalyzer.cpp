#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

const string SyntaxAnalyzer::FUNCTION_KEYWORD = "fun";

const map<string, BinaryExpression::EOperator> SyntaxAnalyzer::BINARY_EXPRESSION_OPERATORS = {
    {"+", BinaryExpression::eAdd}, {"-", BinaryExpression::eSubtract}};

bool SyntaxAnalyzer::Process(const TokenSequence& tokens, SyntaxTreeNode*& syntaxTree)
{
    TokenIterator iter = tokens.cbegin();
    TokenIterator endIter = tokens.cend();

    // TODO: loop and look for multiple function definitions
    FunctionDefinition* functionDefinition = ProcessFunctionDefinition(iter, endIter);

    syntaxTree = functionDefinition;
    return syntaxTree != nullptr;
}

FunctionDefinition* SyntaxAnalyzer::ProcessFunctionDefinition(TokenIterator iter,
                                                              TokenIterator endIter)
{
    if (iter == endIter || iter->GetValue() != FUNCTION_KEYWORD)
    {
        cerr << "Expected function keyword\n";
        return nullptr;
    }

    ++iter;
    if (iter == endIter || !isIdentifier(iter->GetValue()))
    {
        cerr << "\"" << iter->GetValue() << "\" is not a valid function name\n";
        return nullptr;
    }

    string functionName = iter->GetValue();

    ++iter;
    if (iter == endIter || iter->GetValue() != "(")
    {
        cerr << "Expected \"(\"\n";
        return nullptr;
    }

    ++iter;
    vector<VariableDefinition*> parameters;
    bool ok = ProcessParameters(iter, endIter, parameters);
    if (!ok)
    {
        for (auto param : parameters)
        {
            delete param;
        }
        return nullptr;
    }

    ++iter;
    SyntaxTreeNode* code = ProcessExpression(iter, endIter);
    if (code == nullptr)
    {
        for (auto param : parameters)
        {
            delete param;
        }
        return nullptr;
    }

    FunctionDefinition* functionDefinition = new FunctionDefinition(functionName, parameters, code);
    return functionDefinition;
}

bool SyntaxAnalyzer::ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                                       vector<VariableDefinition*>& parameters)
{
    bool expectParam = true;
    parameters.clear();

    while (iter != endIter && iter->GetValue() != ")")
    {
        const string& value = iter->GetValue();
        if (expectParam)
        {
            if (isIdentifier(value))
            {
                VariableDefinition* param = new VariableDefinition(value);
                parameters.push_back(param);
            }
            else
            {
                cerr << "Invalid parameter name: \"" << value << "\"\n";
                return false;
            }
        }
        else
        {
            if (value != ",")
            {
                cerr << "Expected \",\" not \"" << value << "\"\n";
                return false;
            }
        }

        ++iter;
        expectParam = !expectParam;
    }

    if (iter == endIter)
    {
        cerr << "Expected \")\"\n";
        return false;
    }
    else if (expectParam && parameters.size() > 0)
    {
        cerr << "Expected a parameter\n";
        return false;
    }

    return true;
}

Expression* SyntaxAnalyzer::ProcessExpression(TokenIterator& iter, TokenIterator endIter)
{
    Expression* expression = nullptr;
    bool expectNumOrVar = true;
    BinaryExpression::EOperator binOp = BinaryExpression::eAdd;
    while (iter != endIter)
    {
        string value = iter->GetValue();

        if (value == "\n" || value == ";")
        {
            break;
        }

        if (expectNumOrVar)
        {
            if (isNumber(value))
            {
                NumericExpression* numExpr = new NumericExpression(value);
                if (expression == nullptr)
                {
                    expression = numExpr;
                }
                else
                {
                    BinaryExpression* binExpr = new BinaryExpression(binOp, expression, numExpr);
                    expression = binExpr;
                }
            }
            else if (isIdentifier(value))
            {
                VariableExpression* varExpr = new VariableExpression(value);
                if (expression == nullptr)
                {
                    expression = varExpr;
                }
                else
                {
                    BinaryExpression* binExpr = new BinaryExpression(binOp, expression, varExpr);
                    expression = binExpr;
                }
            }
            else
            {
                cerr << "\"" << value << "\" is not a number\n";
                delete expression;
                return nullptr;
            }
        }
        else
        {
            auto opIter = BINARY_EXPRESSION_OPERATORS.find(value);
            if (opIter == BINARY_EXPRESSION_OPERATORS.cend())
            {
                cerr << "Expected an operator, but got \"" << value << "\" instead\n";
                delete expression;
                return nullptr;
            }

            binOp = opIter->second;
        }

        expectNumOrVar = !expectNumOrVar;
        ++iter;
    }

    if (expectNumOrVar)
    {
        cerr << "Expected another number\n";
        delete expression;
        return nullptr;
    }

    return expression;
}
