#ifndef COMPILER_H_
#define COMPILER_H_

#include "CompilerContext.h"
#include "Config.h"
#include "ErrorLogger.h"
#include "Token.h"
#include <istream>
#include <vector>

class Compiler
{
public:
    Compiler();

    Compiler(const Config& config);

    bool Compile();

private:
    CompilerContext compilerContext;
    Config config;
    ErrorLogger logger;

    void PrintTokens(const std::vector<Token>& tokens) const;
};

#endif // COMPILER_H_
