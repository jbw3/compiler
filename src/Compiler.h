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
        bool printSyntaxTree;

        Config()
        {
            printSyntaxTree = false;
        }
    };

    Compiler();

    Compiler(const Config& config);

    bool Compile();

private:
    Config config;
};

#endif // COMPILER_H_
