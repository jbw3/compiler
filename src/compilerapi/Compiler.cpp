#ifdef _MSC_VER
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif

#include "Compiler.h"
#include "AssemblyGenerator.h"
#include "CHeaderPrinter.h"
#include "CompilerContext.h"
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
    compilerContext(config, cerr),
    logger(compilerContext.logger)
{
}

Compiler::Compiler(const Config& config) :
    config(config),
    compilerContext(config, cerr),
    logger(compilerContext.logger)
{
    // initialize types
    TypeInfo::InitTypes(config.targetMachine);

    compilerContext.InitBasicTypes();
}

bool Compiler::CompileSyntaxTree(Modules*& syntaxTree)
{
    SW_CREATE(Lexing);
    SW_CREATE(Syntax);
    SW_CREATE(Semantic);

    bool ok = true;
    syntaxTree = new Modules;

    // lexical analysis
    if (ok)
    {
        SW_START(Lexing);

        for (string filename : config.inFilenames)
        {
            LexicalAnalyzer lexicalAnalyzer(compilerContext, logger);
            ok = lexicalAnalyzer.Process(filename);

            if (!ok)
            {
                break;
            }
        }

        SW_STOP(Lexing);
        SW_PRINT(Lexing);
    }

    // check if tokens are the output
    if (ok && config.emitType == Config::eTokens)
    {
        unsigned fileIdCount = compilerContext.GetFileIdCount();
        for (unsigned fileId = 0; fileId < fileIdCount; ++fileId)
        {
            PrintTokens(compilerContext.GetFileTokens(fileId));
        }
        return ok;
    }

    // syntax analysis
    if (ok)
    {
        SW_START(Syntax);

        SyntaxAnalyzer syntaxAnalyzer(compilerContext, logger);
        ok = syntaxAnalyzer.Process(syntaxTree);

        SW_STOP(Syntax);
        SW_PRINT(Syntax);
    }

    // semantic analysis
    if (ok)
    {
        SW_START(Semantic);

        SemanticAnalyzer semanticAnalyzer(compilerContext, logger);
        ok = semanticAnalyzer.Process(syntaxTree);

        SW_STOP(Semantic);
        SW_PRINT(Semantic);
    }

    return ok;
}

bool Compiler::Compile()
{
    SW_CREATE(IrGen);
    SW_CREATE(IrOpt);
    SW_CREATE(AsmGen);

    Modules* syntaxTree = nullptr;
    bool ok = CompileSyntaxTree(syntaxTree);

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
        CHeaderPrinter printer(logger);
        ok = printer.Print(config, syntaxTree);
        delete syntaxTree;
        return ok;
    }

    if ( ok && (config.emitType == Config::eAssembly || config.emitType == Config::eLlvmIr) )
    {
        llvm::Module* module = nullptr;

        SW_START(IrGen);

        LlvmIrGenerator irGenerator(compilerContext, config, logger);
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

void Compiler::PrintTokens(const TokenList& tokens) const
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
        *os << token.value << '\n';
    }

    if (os != &cout)
    {
        delete os;
    }
}
