#ifndef COMPILER_H_
#define COMPILER_H_

#include "CompilerContext.h"
#include "Config.h"
#include "ErrorLogger.h"
#include "SyntaxTree.h"
#include "Token.h"
#include <istream>

class TokenList;

class Compiler
{
public:
    Compiler();

    Compiler(const Config& config);

    bool CompileSyntaxTree(SyntaxTree::Modules*& syntaxTree);

    bool Compile();

private:
    Config config;
    CompilerContext compilerContext;
    ErrorLogger& logger;

    void PrintTokens(const TokenList& tokens) const;
};

#endif // COMPILER_H_
