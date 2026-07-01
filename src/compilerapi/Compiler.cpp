#include "Compiler.h"
#include "AssemblyGenerator.h"
#include "CHeaderPrinter.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "LlvmIrGenerator.h"
#include "LlvmIrOptimizer.h"
#include "SemanticAnalyzer.h"
#include "StartEndTokenFinder.h"
#include "Stopwatch.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"
#include "SyntaxTreePrinter.h"
#include "llvm/IR/Module.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;
using namespace SyntaxTree;

const char* const BUILD_FILE_NAME = "build.wip";

Compiler::Compiler(const Config& config) :
    compilerContext(config, cerr)
{
}

bool Compiler::CompileSyntaxTree(Modules*& syntaxTree)
{
    SW_CREATE(Lexing);
    SW_CREATE(Syntax);
    SW_CREATE(Semantic);

    bool ok = true;
    syntaxTree = new Modules;

    if (ok)
    {
        ok = CompileBuildFile(syntaxTree);
    }

    // lexical analysis
    if (ok)
    {
        SW_START(Lexing);

        for (string filename : compilerContext.config.inFilenames)
        {
            LexicalAnalyzer lexicalAnalyzer(compilerContext);
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
    if (ok && compilerContext.config.emitType == Config::eTokens)
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

        SyntaxAnalyzer syntaxAnalyzer(compilerContext);
        ok = syntaxAnalyzer.Process(syntaxTree);

        SW_STOP(Syntax);
        SW_PRINT(Syntax);
    }

    // semantic analysis
    if (ok)
    {
        SW_START(Semantic);

        SemanticAnalyzer semanticAnalyzer(compilerContext);
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

    Config::EEmitType emitType = compilerContext.config.emitType;

    // check if syntax tree is the output
    if (ok && emitType == Config::eSyntaxTree)
    {
        SyntaxTreePrinter printer(compilerContext.config.outFilename);
        syntaxTree->Accept(&printer);
        delete syntaxTree;
        return ok;
    }

    // check if C header is the output
    if (ok && emitType == Config::eCHeader)
    {
        CHeaderPrinter printer(compilerContext);
        ok = printer.Print(syntaxTree);
        delete syntaxTree;
        return ok;
    }

    if ( ok && (emitType == Config::eAssembly || emitType == Config::eLlvmIr) )
    {
        llvm::Module* module = nullptr;

        SW_START(IrGen);

        LlvmIrGenerator irGenerator(compilerContext);
        ok = irGenerator.Generate(syntaxTree, module);

        SW_STOP(IrGen);
        SW_PRINT(IrGen);

        if (ok)
        {
            SW_START(IrOpt);

            LlvmOptimizer optimizer(compilerContext.config);
            ok = optimizer.Optimize(module);

            SW_STOP(IrOpt);
            SW_PRINT(IrOpt);
        }

        if (ok)
        {
            SW_START(AsmGen);

            AssemblyGenerator asmGenerator(compilerContext);
            ok = asmGenerator.Generate(module);

            SW_STOP(AsmGen);
            SW_PRINT(AsmGen);
        }

        delete module;
    }

    delete syntaxTree;
    return ok;
}

bool Compiler::CompileBuildFile(SyntaxTree::Modules* syntaxTree)
{
    bool ok = true;

    // lexical analysis
    if (ok)
    {
        string buildFile = compilerContext.config.topDir / BUILD_FILE_NAME;
        LexicalAnalyzer lexicalAnalyzer(compilerContext);
        ok = lexicalAnalyzer.Process(buildFile);

    }

    // syntax analysis
    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer(compilerContext);
        ok = syntaxAnalyzer.Process(syntaxTree);
    }

    // semantic analysis
    if (ok)
    {
        SemanticAnalyzer semanticAnalyzer(compilerContext);
        ok = semanticAnalyzer.Process(syntaxTree);
    }

    // check BuildConfig struct
    if (ok)
    {
        ok = CheckBuildConfigStruct(syntaxTree);
    }

    // check BuildConfigs array
    if (ok)
    {
        ok = CheckBuildConfigsArray(syntaxTree);
    }

    return ok;
}

void Compiler::PrintTokens(const TokenList& tokens) const
{
    ostream* os = nullptr;
    if (compilerContext.config.outFilename.empty())
    {
        os = &cout;
    }
    else
    {
        os = new fstream(compilerContext.config.outFilename, ios_base::out);
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

bool Compiler::CheckBuildConfigStruct(Modules* syntaxTree)
{
    const ConstantDeclaration* buildConfig = nullptr;
    for (const ConstantDeclaration* constDecl : syntaxTree->orderedGlobalConstants)
    {
        if (constDecl->name == "BuildConfig")
        {
            buildConfig = constDecl;
            break;
        }
    }

    if (buildConfig == nullptr)
    {
        compilerContext.logger.LogError("{} does not have a BuildConfig struct", BUILD_FILE_NAME);
        return false;
    }

    unsigned constIdx = buildConfig->assignmentExpression->right->GetConstantValueIndex();
    const TypeInfo* structType = compilerContext.GetTypeConstantValue(constIdx);
    if (!structType->IsStruct())
    {
        StartEndTokenFinder finder;
        buildConfig->assignmentExpression->right->Accept(&finder);

        compilerContext.logger.LogError(*finder.start, *finder.end, "BuildConfig is not a struct");
        return false;
    }

    unordered_map<ROString, const TypeInfo*> expectedMembers =
    {
        {"Name", compilerContext.typeRegistry.GetStringType()},
        {"DebugInfo", TypeInfo::BoolType},
    };

    for (const MemberInfo* member : structType->GetMembers())
    {
        const ROString& name = member->GetName();
        auto iter = expectedMembers.find(name);
        if (iter == expectedMembers.end())
        {
            compilerContext.logger.LogError(*member->GetToken(), "Invalid BuildConfig member '{}'", name);
            return false;
        }
        else if (!member->GetType()->IsSameAs(*iter->second))
        {
            compilerContext.logger.LogError(
                *member->GetToken(),
                "Expected type of '{}' to be '{}'",
                name,
                iter->second->GetName()
            );
            return false;
        }
        else
        {
            expectedMembers.erase(name);
        }
    }

    size_t missingMembersCount = expectedMembers.size();
    if (missingMembersCount > 0)
    {
        stringstream errorMsg;
        if (missingMembersCount == 1)
        {
            errorMsg << "BuildConfig is missing member '" << expectedMembers.cbegin()->first << "'";
        }
        else
        {
            auto iter = expectedMembers.cbegin();
            errorMsg << "BuildConfig is missing the following members: " << iter->first;
            ++iter;
            for (; iter != expectedMembers.cend(); ++iter)
            {
                errorMsg << ", ";
                errorMsg << iter->first;
            }
        }

        StartEndTokenFinder finder;
        buildConfig->assignmentExpression->right->Accept(&finder);

        compilerContext.logger.LogError(
            *finder.start,
            *finder.end,
            errorMsg.str().c_str()
        );
        return false;
    }

    return true;
}

bool Compiler::CheckBuildConfigsArray(Modules* syntaxTree)
{
    const ConstantDeclaration* buildConfigs = nullptr;
    for (const ConstantDeclaration* constDecl : syntaxTree->orderedGlobalConstants)
    {
        if (constDecl->name == "BuildConfigs")
        {
            buildConfigs = constDecl;
            break;
        }
    }

    if (buildConfigs == nullptr)
    {
        compilerContext.logger.LogError("{} does not have a BuildConfigs array", BUILD_FILE_NAME);
        return false;
    }

    return true;
}
