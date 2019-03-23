#ifndef COMPILER_H_
#define COMPILER_H_

#include "Config.h"
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
    Config config;

    void PrintTokens(const std::vector<Token>& tokens) const;
};

#endif // COMPILER_H_
