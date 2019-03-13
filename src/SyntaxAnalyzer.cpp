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

    // TODO: put in support for parameters here

    ++iter;
    if (iter == endIter || iter->GetValue() != ")")
    {
        cerr << "Expected \")\"\n";
        return nullptr;
    }

    ++iter;
    SyntaxTreeNode* code = ProcessExpression(iter, endIter);
    if (code == nullptr)
    {
        return nullptr;
    }

    FunctionDefinition* functionDefinition = new FunctionDefinition(functionName, code);
    return functionDefinition;
}

Expression* SyntaxAnalyzer::ProcessExpression(TokenIterator iter, TokenIterator endIter)
{
    Expression* expression = nullptr;
    bool expectNumber = true;
    BinaryExpression::EOperator binOp = BinaryExpression::eAdd;
    while (iter != endIter)
    {
        string value = iter->GetValue();

        if (value == "\n" || value == ";")
        {
            break;
        }

        if (expectNumber)
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

        expectNumber = !expectNumber;
        ++iter;
    }

    if (expectNumber)
    {
        cerr << "Expected another number\n";
        delete expression;
        return nullptr;
    }

    return expression;
}
