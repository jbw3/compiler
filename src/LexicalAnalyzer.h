#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "Token.h"
#include <istream>
#include <unordered_set>
#include <vector>

class LexicalAnalyzer
{
public:
    LexicalAnalyzer();

    std::vector<Token> Process(std::istream& is);

private:
    static const std::unordered_set<std::string> SYMBOLS;

    std::string tokenStr;
    bool isValid;
    unsigned long line;
    unsigned long column;

    Token CreateToken();

    void ParseChar(char ch, std::vector<Token>& tokens);

    bool IsValidToken(const std::string& str) const;

    void ThrowError();
};

#endif // LEXICAL_ANALYZER_H_
