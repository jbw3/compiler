#include "Compiler.h"
#include "Error.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"
#include "SyntaxTreePrinter.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

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
    SyntaxTreeNode* syntaxTree = syntaxAnalyzer.Process(tokens);

    SyntaxTreePrinter printer;
    syntaxTree->Accept(&printer);

    delete syntaxTree;
}
