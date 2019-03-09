#include "Compiler.h"
#include "LexicalAnalyzer.h"
#include "LlvmIrGenerator.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"
#include "SyntaxTreePrinter.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

Compiler::Compiler()
{
}

Compiler::Compiler(const Compiler::Config& config) : config(config)
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
        if (config.printSyntaxTree)
        {
            SyntaxTreePrinter printer;
            syntaxTree->Accept(&printer);
        }

        LlvmIrGenerator generator;
        ok = generator.GenerateCode(syntaxTree);
    }

    delete syntaxTree;
    return ok;
}
