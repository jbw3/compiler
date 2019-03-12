#ifndef COMPILER_H_
#define COMPILER_H_

#include "Token.h"
#include <istream>
#include <vector>

class Compiler
{
public:
    struct Config
    {
        enum EOutput
        {
            eTokens,
            eSyntaxTree,
            eIr,
        };

        EOutput output;

        Config()
        {
            output = eIr;
        }
    };

    Compiler();

    Compiler(const Config& config);

    bool Compile();

private:
    Config config;

    void PrintTokens(const std::vector<Token>& tokens) const;
};

#endif // COMPILER_H_
