#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "ErrorLogger.h"
#include "Token.h"
#include <unordered_set>
#include <vector>

class LexicalAnalyzer
{
public:
    LexicalAnalyzer(ErrorLogger& logger);

    bool Process(const std::string& inFile, std::vector<Token>& tokens);

    bool Process(std::istream& is, std::vector<Token>& tokens);

private:
    static const char COMMENT_START;

    static const char BLOCK_COMMENT_INNER;

    static const char LINE_COMMENT_END;

    static const std::unordered_set<std::string> SYMBOLS;

    ErrorLogger& logger;
    std::string tokenStr;
    bool isValid;
    bool isString;
    std::string filename;
    unsigned long line;
    unsigned long column;

    unsigned long GetTokenStrStartColumn();

    Token CreateToken(unsigned long columnNum = 0);

    bool ParseChar(char ch, std::vector<Token>& tokens);

    bool IsValidToken(const std::string& str) const;

    bool AddTokenIfValid(std::vector<Token>& tokens);

    void ParseLineComment(std::istream& is);

    bool ParseBlockComment(std::istream& is);

    void PrintError();
};

#endif // LEXICAL_ANALYZER_H_
