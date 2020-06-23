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
#include "timing.h"
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
        START_TIMER(Lexing);

        LexicalAnalyzer lexicalAnalyzer(logger);
        ok = lexicalAnalyzer.Process(config.inFilename, tokens);

        END_TIMER(Lexing);
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
        START_TIMER(Syntax);

        SyntaxAnalyzer syntaxAnalyzer(logger);
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);

        END_TIMER(Syntax);
    }

    // semantic analysis
    if (ok)
    {
        START_TIMER(Semantic);

        SemanticAnalyzer semanticAnalyzer(logger);
        ok = semanticAnalyzer.Process(syntaxTree);

        END_TIMER(Semantic);
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

        START_TIMER(IrGen);

        LlvmIrGenerator irGenerator(config, logger);
        ok = irGenerator.Generate(syntaxTree, module);

        END_TIMER(IrGen);

        if (ok)
        {
            START_TIMER(IrOpt);

            LlvmOptimizer optimizer(config);
            ok = optimizer.Optimize(module);

            END_TIMER(IrOpt);
        }

        if (ok)
        {
            START_TIMER(AsmGen);

            AssemblyGenerator asmGenerator(config, logger);
            ok = asmGenerator.Generate(module);

            END_TIMER(AsmGen);
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
