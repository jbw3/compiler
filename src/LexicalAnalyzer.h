#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "Token.h"
#include <istream>
#include <vector>

class LexicalAnalyzer
{
public:
    std::vector<Token> Process(std::istream& is);

private:
    std::string tokenStr;
    unsigned long line;
    unsigned long column;

    Token CreateToken();
};

#endif // LEXICAL_ANALYZER_H_
