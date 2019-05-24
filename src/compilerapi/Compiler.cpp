#include "Compiler.h"
#include "LexicalAnalyzer.h"
#include "LlvmIrGenerator.h"
#include "SemanticAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"
#include "SyntaxTreePrinter.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

Compiler::Compiler() :
    logger(&cerr)
{
}

Compiler::Compiler(const Config& config) :
    config(config),
    logger(&cerr)
{
}

bool Compiler::Compile()
{
    bool ok = true;

    // lexical analysis
    vector<Token> tokens;
    if (ok)
    {
        LexicalAnalyzer lexicalAnalyzer(logger);
        ok = lexicalAnalyzer.Process(config.inFilename, tokens);
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
        SyntaxAnalyzer syntaxAnalyzer(logger);
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);
    }

    // semantic analysis
    if (ok)
    {
        SemanticAnalyzer semanticAnalyzer;
        ok = semanticAnalyzer.Process(syntaxTree);
    }

    // check if syntax tree is the output
    if (ok && config.outputType == Config::eSyntaxTree)
    {
        SyntaxTreePrinter printer(config.outFilename);
        syntaxTree->Accept(&printer);
        delete syntaxTree;
        return ok;
    }

    if (ok && config.outputType == Config::eAssembly)
    {
        LlvmIrGenerator generator(config);
        ok = generator.GenerateCode(syntaxTree);
    }

    delete syntaxTree;
    return ok;
}

void Compiler::PrintTokens(const vector<Token>& tokens) const
{
    ostream* os = nullptr;
    if (config.outFilename.empty())
    {
        os = &cout;
    }
    else
    {
        os = new fstream(config.outFilename, ios_base::out);
    }

    if (tokens.size() > 0)
    {
        *os << '|';
        for (const Token& token : tokens)
        {
            const string& value = token.GetValue();
            if (value == "\n")
            {
                *os << "\\n";
            }
            else
            {
                *os << value;
            }
            *os << '|';
        }
        *os << '\n';
    }

    if (os != &cout)
    {
        delete os;
    }
}
