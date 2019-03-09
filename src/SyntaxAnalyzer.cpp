#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

const map<string, BinaryExpression::EOperator> SyntaxAnalyzer::BINARY_EXPRESSION_OPERATORS = {
    {"+", BinaryExpression::eAdd}, {"-", BinaryExpression::eSubtract}};

bool SyntaxAnalyzer::Process(const vector<Token>& tokens, SyntaxTreeNode*& syntaxTree)
{
    syntaxTree = nullptr;

    // build syntax tree
    if (tokens.size() < 4)
    {
        cerr << "Not enough tokens\n";
        return false;
    }

    vector<Token>::const_iterator iter = tokens.cbegin();

    string varName = (iter++)->GetValue();
    string assignmentOp = (iter++)->GetValue();

    if (!isIdentifier(varName))
    {
        cerr << "First token is not an identifier\n";
        return false;
    }
    if (assignmentOp != "=")
    {
        cerr << "Expected assignment operator (=)\n";
        return false;
    }

    Expression* rightHandExpr = nullptr;
    bool expectNumber = true;
    BinaryExpression::EOperator binOp = BinaryExpression::eAdd;
    while (iter != tokens.cend())
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
                if (rightHandExpr == nullptr)
                {
                    rightHandExpr = numExpr;
                }
                else
                {
                    BinaryExpression* binExpr = new BinaryExpression(binOp, rightHandExpr, numExpr);
                    rightHandExpr = binExpr;
                }
            }
            else
            {
                cerr << "\"" << value << "\" is not a number\n";
                delete rightHandExpr;
                return false;
            }
        }
        else
        {
            auto opIter = BINARY_EXPRESSION_OPERATORS.find(value);
            if (opIter == BINARY_EXPRESSION_OPERATORS.cend())
            {
                cerr << "Expected an operator, but got \"" << value << "\" instead\n";
                delete rightHandExpr;
                return false;
            }

            binOp = opIter->second;
        }

        expectNumber = !expectNumber;
        ++iter;
    }

    if (expectNumber)
    {
        cerr << "Expected another number\n";
        delete rightHandExpr;
        return false;
    }

    Variable* variable = new Variable(varName);
    Assignment* assignment = new Assignment(variable, rightHandExpr);

    syntaxTree = assignment;
    return true;
}
