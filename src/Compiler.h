#ifndef COMPILER_H_
#define COMPILER_H_

#include "Token.h"
#include <istream>
#include <vector>

class Compiler
{
public:
    Compiler();

    void Compile();

private:
    void PrintTokens(const std::vector<Token>& tokens);
};

#endif // COMPILER_H_
