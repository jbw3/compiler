#ifndef COMPILER_H_
#define COMPILER_H_

#include "CompilerContext.h"
#include "Config.h"
#include "SyntaxTree.h"

class TokenList;

class Compiler
{
public:
    Compiler(const Config& config);

    bool CompileSyntaxTree(SyntaxTree::Modules*& syntaxTree);

    bool Compile();

private:
    CompilerContext compilerContext;

    bool CompileBuildFile(SyntaxTree::Modules* syntaxTree);

    bool CheckBuildConfigStruct(SyntaxTree::Modules* syntaxTree, const TypeInfo*& buildConfigType);

    bool CheckBuildConfigsArray(SyntaxTree::Modules* syntaxTree, const TypeInfo* buildConfigType);

    void PrintTokens(const TokenList& tokens) const;
};

#endif // COMPILER_H_
