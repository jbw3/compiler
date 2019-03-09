#include "Compiler.h"
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

bool Compiler::Compile()
{
    bool ok = true;

    vector<Token> tokens;
    if (ok)
    {
        LexicalAnalyzer lexicalAnalyzer;
        ok = lexicalAnalyzer.Process(std::cin, tokens);
    }

    SyntaxTreeNode* syntaxTree = nullptr;
    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer;
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);
    }

    if (ok)
    {
        SyntaxTreePrinter printer;
        syntaxTree->Accept(&printer);
    }

    delete syntaxTree;
    return ok;
}
