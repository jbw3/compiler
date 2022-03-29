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
    struct TokenValue
    {
        ROString str;
        Token::EType type;
    };

    struct Bucket
    {
        const TokenValue* values;
        size_t size;
    };

    LexicalAnalyzer(CompilerContext& compilerContext);

    bool Process(const std::string& inFile);

    bool Process(CharBuffer buff, TokenList& tokens);

private:
    static const char COMMENT_START;

    static const char BLOCK_COMMENT_INNER;

    static const char LINE_COMMENT_END;

    static const std::unordered_set<char> SYMBOL_START_CHAR;

    // TODO: remove
    static const std::unordered_map<ROString, Token::EType> SYMBOLS;

    // TODO: remove
    static const std::unordered_map<ROString, Token::EType> KEYWORDS;

    static constexpr size_t SYMBOLS_HASH_TABLE_SIZE = 32;
    static const Bucket SYMBOLS_HASH_TABLE[SYMBOLS_HASH_TABLE_SIZE];

    static constexpr size_t KEYWORDS_HASH_TABLE_SIZE = 26;
    static const Bucket KEYWORDS_HASH_TABLE[KEYWORDS_HASH_TABLE_SIZE];

    static Token::EType GetSymbolType(ROString str);

    static Token::EType GetKeywordType(ROString str);

    CompilerContext& compilerContext;
    size_t buffIdx;
    bool isMore;
    ErrorLogger& logger;
    unsigned filenameId;
    unsigned line;
    unsigned column;

    char Read(CharBuffer buff);

    char Peek(CharBuffer buff);

    void ParseLineComment(CharBuffer buff);

    bool ParseBlockComment(CharBuffer buff);
};

#endif // LEXICAL_ANALYZER_H_
