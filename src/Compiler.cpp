#include "Compiler.h"
#include "Error.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include <iostream>

using namespace std;

Compiler::Compiler()
{
}

void Compiler::Compile()
{
    try
    {
        Process();
    }
    catch (const Error&)
    {
    }
}

void Compiler::Process()
{
    LexicalAnalyzer lexicalAnalyzer;
    vector<Token> tokens = lexicalAnalyzer.Process(std::cin);

    SyntaxAnalyzer syntaxAnalyzer;
    syntaxAnalyzer.Process(tokens);
}
