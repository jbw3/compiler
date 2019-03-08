#include "SyntaxAnalyzer.h"
#include "Error.h"
#include "SyntaxTree.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

const unordered_set<string> SyntaxAnalyzer::BINARY_EXPRESSION_OPERATORS = {"+"};

SyntaxTreeNode* SyntaxAnalyzer::Process(const vector<Token>& tokens)
{
    // print tokens
    if (tokens.size() > 0)
    {
        cout << '|';
    }

    for (Token token : tokens)
    {
        string value = token.GetValue();
        cout << (value == "\n" ? "\\n" : value) << '|';
    }

    cout << '\n';

    // build syntax tree
    if (tokens.size() < 4)
    {
        cerr << "Not enough tokens\n";
        throw Error();
    }

    vector<Token>::const_iterator iter = tokens.cbegin();

    string varName = (iter++)->GetValue();
    string assignmentOp = (iter++)->GetValue();

    if (!isIdentifier(varName))
    {
        cerr << "First token is not an identifier\n";
        throw Error();
    }
    if (assignmentOp != "=")
    {
        cerr << "Expected assignment operator (=)\n";
        throw Error();
    }

    Expression* rightHandExpr = nullptr;
    bool expectNumber = true;
    BinaryExpression::EOperator binOp = BinaryExpression::eAddition;
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
                throw Error();
            }
        }
        else
        {
            // TODO: Don't hard-code addition operator
            if (value != "+")
            {
                cerr << "Expected \"+\", but got \"" << value << "\" instead\n";
                delete rightHandExpr;
                throw Error();
            }
            binOp = BinaryExpression::eAddition;
        }

        expectNumber = !expectNumber;
        ++iter;
    }

    if (expectNumber)
    {
        cerr << "Expected another number\n";
        delete rightHandExpr;
        throw Error();
    }

    Variable* variable = new Variable(varName);
    Assignment* assignment = new Assignment(variable, rightHandExpr);

    return assignment;
}
