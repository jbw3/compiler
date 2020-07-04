#include "Compiler.h"
#include "AssemblyGenerator.h"
#include "CHeaderPrinter.h"
#include "LexicalAnalyzer.h"
#include "LlvmIrGenerator.h"
#include "LlvmIrOptimizer.h"
#include "SemanticAnalyzer.h"
#include "Stopwatch.h"
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
    SW_CREATE(Lexing);
    SW_CREATE(Syntax);
    SW_CREATE(Semantic);
    SW_CREATE(IrGen);
    SW_CREATE(IrOpt);
    SW_CREATE(AsmGen);

    bool ok = true;

    // lexical analysis
    vector<Token> tokens;
    if (ok)
    {
        SW_START(Lexing);

        LexicalAnalyzer lexicalAnalyzer(logger);
        ok = lexicalAnalyzer.Process(config.inFilename, tokens);

        SW_STOP(Lexing);
        SW_PRINT(Lexing);
    }

    // check tokens are the output
    if (ok && config.emitType == Config::eTokens)
    {
        PrintTokens(tokens);
        return ok;
    }

    // syntax analysis
    ModuleDefinition* syntaxTree = nullptr;
    if (ok)
    {
        SW_START(Syntax);

        SyntaxAnalyzer syntaxAnalyzer(logger);
        ok = syntaxAnalyzer.Process(tokens, syntaxTree);

        SW_STOP(Syntax);
        SW_PRINT(Syntax);
    }

    // semantic analysis
    if (ok)
    {
        SW_START(Semantic);

        SemanticAnalyzer semanticAnalyzer(logger);
        ok = semanticAnalyzer.Process(syntaxTree);

        SW_STOP(Semantic);
        SW_PRINT(Semantic);
    }

    // check if syntax tree is the output
    if (ok && config.emitType == Config::eSyntaxTree)
    {
        SyntaxTreePrinter printer(config.outFilename);
        syntaxTree->Accept(&printer);
        delete syntaxTree;
        return ok;
    }

    // check if C header is the output
    if (ok && config.emitType == Config::eCHeader)
    {
        CHeaderPrinter printer;
        ok = printer.Print(config, syntaxTree);
        delete syntaxTree;
        return ok;
    }

    if ( ok && (config.emitType == Config::eAssembly || config.emitType == Config::eLlvmIr) )
    {
        llvm::Module* module = nullptr;

        SW_START(IrGen);

        LlvmIrGenerator irGenerator(config, logger);
        ok = irGenerator.Generate(syntaxTree, module);

        SW_STOP(IrGen);
        SW_PRINT(IrGen);

        if (ok)
        {
            SW_START(IrOpt);

            LlvmOptimizer optimizer(config);
            ok = optimizer.Optimize(module);

            SW_STOP(IrOpt);
            SW_PRINT(IrOpt);
        }

        if (ok)
        {
            SW_START(AsmGen);

            AssemblyGenerator asmGenerator(config, logger);
            ok = asmGenerator.Generate(module);

            SW_STOP(AsmGen);
            SW_PRINT(AsmGen);
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
