#include "SyntaxAnalyzer.h"
#include "Error.h"
#include "SyntaxTree.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

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

    string varName = tokens[0].GetValue();
    string op = tokens[1].GetValue();
    string number = tokens[2].GetValue();
    string end = tokens[3].GetValue();

    if (!isIdentifier(varName))
    {
        cerr << "First token is not an identifier\n";
        throw Error();
    }
    if (op != "=")
    {
        cerr << "Expected assignment operator (=)\n";
    }
    if (!isNumber(number))
    {
        cerr << "\"" << number << "\" is not a number\n";
    }
    if (end != "\n" && end != ";")
    {
        cerr << "Expected end of statement\n";
    }

    Variable* variable = new Variable(varName);
    NumericExpression* numericExpression = new NumericExpression(number);
    Assignment* assignment = new Assignment(variable, numericExpression);

    return assignment;
}
