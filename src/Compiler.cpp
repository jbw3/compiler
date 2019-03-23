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

Compiler::Compiler(const Config& config) : config(config)
{
}

bool Compiler::Compile()
{
    bool ok = true;

    // lexical analysis
    vector<Token> tokens;
    if (ok)
    {
        LexicalAnalyzer lexicalAnalyzer;
        ok = lexicalAnalyzer.Process(std::cin, tokens);
    }

    // check tokens are the output
    if (ok && config.outputType == Config::eTokens)
    {
        PrintTokens(tokens);
        return ok;
    }

    // syntax analysis
    SyntaxTreeNode* syntaxTree = nullptr;
    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer;
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);
    }

    // check if syntax tree is the output
    if (ok && config.outputType == Config::eSyntaxTree)
    {
        SyntaxTreePrinter printer;
        syntaxTree->Accept(&printer);
        delete syntaxTree;
        return ok;
    }

    if (ok && config.outputType == Config::eAssembly)
    {
        LlvmIrGenerator generator;
        ok = generator.GenerateCode(syntaxTree);
    }

    delete syntaxTree;
    return ok;
}

void Compiler::PrintTokens(const std::vector<Token>& tokens) const
{
    if (tokens.size() > 0)
    {
        cout << '|';
        for (const Token& token : tokens)
        {
            const string& value = token.GetValue();
            if (value == "\n")
            {
                cout << "\\n";
            }
            else
            {
                cout << value;
            }
            cout << '|';
        }
        cout << '\n';
    }
}
