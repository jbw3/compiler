#ifndef COMPILER_H_
#define COMPILER_H_

#include "Token.h"
#include <istream>
#include <vector>

class Compiler
{
public:
    Compiler();

    bool Compile();
};

#endif // COMPILER_H_
