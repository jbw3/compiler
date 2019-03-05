#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "Token.h"
#include <vector>

class SyntaxAnalyzer
{
public:
    void Process(const std::vector<Token>& tokens);
};

#endif // SYNTAX_ANALYZER_H_
