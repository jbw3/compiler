#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "Token.h"
#include <unordered_set>
#include <vector>

class LexicalAnalyzer
{
public:
    LexicalAnalyzer();

    bool Process(const std::string& inFile, std::vector<Token>& tokens);

    bool Process(std::istream& is, std::vector<Token>& tokens);

private:
    static const std::unordered_set<std::string> SYMBOLS;

    std::string tokenStr;
    bool isValid;
    unsigned long line;
    unsigned long column;

    Token CreateToken();

    bool ParseChar(char ch, std::vector<Token>& tokens);

    bool IsValidToken(const std::string& str) const;

    void PrintError();
};

#endif // LEXICAL_ANALYZER_H_
