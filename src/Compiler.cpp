#include "Compiler.h"
#include "LexicalAnalyzer.h"
#include <iostream>

using namespace std;

Compiler::Compiler()
{
}

void Compiler::Compile()
{
    LexicalAnalyzer lexicalAnalyzer;
    vector<Token> tokens = lexicalAnalyzer.Process(std::cin);

    PrintTokens(tokens);
}

void Compiler::PrintTokens(const vector<Token>& tokens)
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
