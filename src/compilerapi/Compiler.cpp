#include "Compiler.h"
#include "AssemblyGenerator.h"
#include "CHeaderPrinter.h"
#include "LexicalAnalyzer.h"
#include "LlvmIrGenerator.h"
#include "LlvmIrOptimizer.h"
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
    // initialize types
    TypeInfo::InitTypes(config.targetMachine);
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
    ModuleDefinition* syntaxTree = nullptr;
    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer(logger);
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);
    }

    // semantic analysis
    if (ok)
    {
        SemanticAnalyzer semanticAnalyzer(logger);
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

    // check if C header is the output
    if (ok && config.outputType == Config::eCHeader)
    {
        CHeaderPrinter printer;
        ok = printer.Print(config, syntaxTree);
        delete syntaxTree;
        return ok;
    }

    if (ok && config.outputType == Config::eAssembly)
    {
        llvm::Module* module = nullptr;

        LlvmIrGenerator irGenerator(config, logger);
        ok = irGenerator.Generate(syntaxTree, module);

        if (ok)
        {
            LlvmOptimizer optimizer(config);
            ok = optimizer.Optimize(module);
        }

        if (ok)
        {
            AssemblyGenerator asmGenerator(config, logger);
            ok = asmGenerator.Generate(module);
        }

        delete module;
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

    for (const Token& token : tokens)
    {
        *os << token.GetValue() << '\n';
    }

    if (os != &cout)
    {
        delete os;
    }
}
