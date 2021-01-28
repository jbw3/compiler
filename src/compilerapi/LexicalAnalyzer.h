#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "ErrorLogger.h"
#include "Token.h"
#include "utils.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CompilerContext;

class LexicalAnalyzer
{
public:
    LexicalAnalyzer(CompilerContext& compilerContext, ErrorLogger& logger);

    ~LexicalAnalyzer();

    bool Process(const std::string& inFile, std::vector<Token>& tokens);

    bool Process(std::istream& is, std::vector<Token>& tokens);

private:
    static const char COMMENT_START;

    static const char BLOCK_COMMENT_INNER;

    static const char LINE_COMMENT_END;

    static const std::unordered_set<char> SYMBOL_START_CHAR;

    static const std::unordered_map<const char*, Token::EType, CStringHash, CStringEqual> SYMBOLS;

    static const std::unordered_map<const char*, Token::EType, CStringHash, CStringEqual> KEYWORDS;

    static constexpr size_t MAX_BUFF_CAPACITY = 1024;
    CompilerContext& compilerContext;
    char* buff;
    size_t buffSize;
    size_t buffIdx;
    bool isMore;
    ErrorLogger& logger;
    std::string tokenStr;
    std::string filename;
    unsigned filenameId;
    unsigned line;
    unsigned column;

    char Read(std::istream& is);

    void ParseLineComment(std::istream& is);

    bool ParseBlockComment(std::istream& is);
};

#endif // LEXICAL_ANALYZER_H_
