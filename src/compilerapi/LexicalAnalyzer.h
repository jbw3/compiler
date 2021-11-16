#ifndef LEXICAL_ANALYZER_H_
#define LEXICAL_ANALYZER_H_

#include "CompilerContext.h"
#include "Token.h"
#include "utils.h"
#include <unordered_map>
#include <unordered_set>

class LexicalAnalyzer
{
public:
    LexicalAnalyzer(CompilerContext& compilerContext);

    bool Process(const std::string& inFile);

    bool Process(CharBuffer buff, TokenList& tokens);

private:
    static const char COMMENT_START;

    static const char BLOCK_COMMENT_INNER;

    static const char LINE_COMMENT_END;

    static const std::unordered_set<char> SYMBOL_START_CHAR;

    static const std::unordered_map<const char*, Token::EType, CStringHash, CStringEqual> SYMBOLS;

    static const std::unordered_map<const char*, Token::EType, CStringHash, CStringEqual> KEYWORDS;

    static constexpr size_t MAX_BUFF_CAPACITY = 1024;
    CompilerContext& compilerContext;
    size_t buffIdx;
    bool isMore;
    ErrorLogger& logger;
    unsigned filenameId;
    unsigned line;
    unsigned column;

    char Read(CharBuffer buff);

    void ParseLineComment(CharBuffer buff);

    bool ParseBlockComment(CharBuffer buff);
};

#endif // LEXICAL_ANALYZER_H_
