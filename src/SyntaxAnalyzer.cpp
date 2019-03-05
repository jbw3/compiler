#include "SyntaxAnalyzer.h"
#include <iostream>

using namespace std;

void SyntaxAnalyzer::Process(const vector<Token>& tokens)
{
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
}
