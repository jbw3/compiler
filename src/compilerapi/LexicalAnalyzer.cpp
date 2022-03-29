#include "LexicalAnalyzer.h"
#include "keywords.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

const char LexicalAnalyzer::COMMENT_START = '#';

const char LexicalAnalyzer::BLOCK_COMMENT_INNER = '!';

const char LexicalAnalyzer::LINE_COMMENT_END = '\n';

const unordered_set<char> LexicalAnalyzer::SYMBOL_START_CHAR =
{
    '=', '!', '<', '>', '+', '-', '*', '/', '%', '&', '|', '^', '.', ',', ';', ':', '(', ')', '[', ']', '{', '}',
};

const unordered_map<ROString, Token::EType> LexicalAnalyzer::SYMBOLS =
{
    { "==", Token::eEqualEqual },
    { "!=", Token::eExclaimEqual },
    { "<", Token::eLess },
    { "<=", Token::eLessEqual },
    { ">", Token::eGreater },
    { ">=", Token::eGreaterEqual },
    { "+", Token::ePlus },
    { "-", Token::eMinus },
    { "*", Token::eTimes },
    { "/", Token::eDivide },
    { "%", Token::eRemainder },
    { "<<", Token::eLessLess },
    { ">>", Token::eGreaterGreater },
    { ">>>", Token::eGreaterGreaterGreater },
    { "!", Token::eExclaim },
    { "&", Token::eAmpersand },
    { "^", Token::eCaret },
    { "|", Token::eBar },
    { "&&", Token::eAmpersandAmpersand },
    { "||", Token::eBarBar },
    { "(", Token::eOpenPar },
    { ")", Token::eClosePar },
    { "[", Token::eOpenBracket },
    { "]", Token::eCloseBracket },
    { "{", Token::eOpenBrace },
    { "}", Token::eCloseBrace },
    { ",", Token::eComma },
    { ".", Token::ePeriod },
    { ";", Token::eSemiColon },
    { ":", Token::eColon },
    { "=", Token::eEqual },
    { "+=", Token::ePlusEqual },
    { "-=", Token::eMinusEqual },
    { "*=", Token::eTimesEqual },
    { "/=", Token::eDivideEqual },
    { "%=", Token::eRemainderEqual },
    { "<<=", Token::eLessLessEqual },
    { ">>=", Token::eGreaterGreaterEqual },
    { ">>>=", Token::eGreaterGreaterGreaterEqual },
    { "&=", Token::eAmpersandEqual },
    { "^=", Token::eCaretEqual },
    { "|=", Token::eBarEqual },
    { "..", Token::ePeriodPeriod },
    { "..<", Token::ePeriodPeriodLess },
};

const unordered_map<ROString, Token::EType> LexicalAnalyzer::KEYWORDS =
{
    { BOOL_KEYWORD, Token::eBool },
    { BREAK_KEYWORD, Token::eBreak },
    { CAST_KEYWORD, Token::eCast },
    { CONST_KEYWORD, Token::eConst },
    { CONTINUE_KEYWORD, Token::eContinue },
    { ELIF_KEYWORD, Token::eElif },
    { ELSE_KEYWORD, Token::eElse },
    { EXTERN_KEYWORD, Token::eExtern },
    { FALSE_KEYWORD, Token::eFalseLit },
    { FOR_KEYWORD, Token::eFor },
    { FUNCTION_KEYWORD, Token::eFun },
    { IF_KEYWORD, Token::eIf },
    { INT8_KEYWORD, Token::eI8 },
    { INT16_KEYWORD, Token::eI16 },
    { INT32_KEYWORD, Token::eI32 },
    { INT64_KEYWORD, Token::eI64 },
    { IN_KEYWORD, Token::eIn },
    { INT_SIZE_KEYWORD, Token::eISize },
    { RETURN_KEYWORD, Token::eReturn },
    { STR_KEYWORD, Token::eStr },
    { STRUCT_KEYWORD, Token::eStruct },
    { TRUE_KEYWORD, Token::eTrueLit },
    { TYPE_KEYWORD, Token::eType },
    { UINT8_KEYWORD, Token::eU8 },
    { UINT16_KEYWORD, Token::eU16 },
    { UINT32_KEYWORD, Token::eU32 },
    { UINT64_KEYWORD, Token::eU64 },
    { UINT_SIZE_KEYWORD, Token::eUSize },
    { VARIABLE_KEYWORD, Token::eVar },
    { WHILE_KEYWORD, Token::eWhile },
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES1[] =
{
    LexicalAnalyzer::TokenValue{ROString("!="), Token::eExclaimEqual},
    LexicalAnalyzer::TokenValue{ROString("!"), Token::eExclaim},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES5[] =
{
    LexicalAnalyzer::TokenValue{ROString("%"), Token::eRemainder},
    LexicalAnalyzer::TokenValue{ROString("%="), Token::eRemainderEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES6[] =
{
    LexicalAnalyzer::TokenValue{ROString("&"), Token::eAmpersand},
    LexicalAnalyzer::TokenValue{ROString("&&"), Token::eAmpersandAmpersand},
    LexicalAnalyzer::TokenValue{ROString("&="), Token::eAmpersandEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES8[] =
{
    LexicalAnalyzer::TokenValue{ROString("("), Token::eOpenPar},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES9[] =
{
    LexicalAnalyzer::TokenValue{ROString(")"), Token::eClosePar},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES10[] =
{
    LexicalAnalyzer::TokenValue{ROString("*"), Token::eTimes},
    LexicalAnalyzer::TokenValue{ROString("*="), Token::eTimesEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES11[] =
{
    LexicalAnalyzer::TokenValue{ROString("+"), Token::ePlus},
    LexicalAnalyzer::TokenValue{ROString("+="), Token::ePlusEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES12[] =
{
    LexicalAnalyzer::TokenValue{ROString(","), Token::eComma},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES13[] =
{
    LexicalAnalyzer::TokenValue{ROString("-"), Token::eMinus},
    LexicalAnalyzer::TokenValue{ROString("-="), Token::eMinusEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES14[] =
{
    LexicalAnalyzer::TokenValue{ROString("."), Token::ePeriod},
    LexicalAnalyzer::TokenValue{ROString(".."), Token::ePeriodPeriod},
    LexicalAnalyzer::TokenValue{ROString("..<"), Token::ePeriodPeriodLess},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES15[] =
{
    LexicalAnalyzer::TokenValue{ROString("/"), Token::eDivide},
    LexicalAnalyzer::TokenValue{ROString("/="), Token::eDivideEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES26[] =
{
    LexicalAnalyzer::TokenValue{ROString(":"), Token::eColon},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES27[] =
{
    LexicalAnalyzer::TokenValue{ROString("["), Token::eOpenBracket},
    LexicalAnalyzer::TokenValue{ROString("{"), Token::eOpenBrace},
    LexicalAnalyzer::TokenValue{ROString(";"), Token::eSemiColon},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES28[] =
{
    LexicalAnalyzer::TokenValue{ROString("<"), Token::eLess},
    LexicalAnalyzer::TokenValue{ROString("<="), Token::eLessEqual},
    LexicalAnalyzer::TokenValue{ROString("<<"), Token::eLessLess},
    LexicalAnalyzer::TokenValue{ROString("|"), Token::eBar},
    LexicalAnalyzer::TokenValue{ROString("||"), Token::eBarBar},
    LexicalAnalyzer::TokenValue{ROString("<<="), Token::eLessLessEqual},
    LexicalAnalyzer::TokenValue{ROString("|="), Token::eBarEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES29[] =
{
    LexicalAnalyzer::TokenValue{ROString("=="), Token::eEqualEqual},
    LexicalAnalyzer::TokenValue{ROString("]"), Token::eCloseBracket},
    LexicalAnalyzer::TokenValue{ROString("}"), Token::eCloseBrace},
    LexicalAnalyzer::TokenValue{ROString("="), Token::eEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES30[] =
{
    LexicalAnalyzer::TokenValue{ROString(">"), Token::eGreater},
    LexicalAnalyzer::TokenValue{ROString(">="), Token::eGreaterEqual},
    LexicalAnalyzer::TokenValue{ROString(">>"), Token::eGreaterGreater},
    LexicalAnalyzer::TokenValue{ROString(">>>"), Token::eGreaterGreaterGreater},
    LexicalAnalyzer::TokenValue{ROString("^"), Token::eCaret},
    LexicalAnalyzer::TokenValue{ROString(">>="), Token::eGreaterGreaterEqual},
    LexicalAnalyzer::TokenValue{ROString(">>>="), Token::eGreaterGreaterGreaterEqual},
    LexicalAnalyzer::TokenValue{ROString("^="), Token::eCaretEqual},
};


const LexicalAnalyzer::Bucket LexicalAnalyzer::SYMBOLS_HASH_TABLE[SYMBOLS_HASH_TABLE_SIZE] =
{
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES1, 2},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES5, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES6, 3},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES8, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES9, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES10, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES11, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES12, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES13, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES14, 3},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES15, 2},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES26, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES27, 3},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES28, 7},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES29, 4},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES30, 8},
    LexicalAnalyzer::Bucket{nullptr, 0},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES1[] =
{
    LexicalAnalyzer::TokenValue{ROString("break"), Token::eBreak},
    LexicalAnalyzer::TokenValue{ROString("bool"), Token::eBool},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES2[] =
{
    LexicalAnalyzer::TokenValue{ROString("continue"), Token::eContinue},
    LexicalAnalyzer::TokenValue{ROString("const"), Token::eConst},
    LexicalAnalyzer::TokenValue{ROString("cast"), Token::eCast},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES4[] =
{
    LexicalAnalyzer::TokenValue{ROString("extern"), Token::eExtern},
    LexicalAnalyzer::TokenValue{ROString("else"), Token::eElse},
    LexicalAnalyzer::TokenValue{ROString("elif"), Token::eElif},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES5[] =
{
    LexicalAnalyzer::TokenValue{ROString("fun"), Token::eFun},
    LexicalAnalyzer::TokenValue{ROString("for"), Token::eFor},
    LexicalAnalyzer::TokenValue{ROString("false"), Token::eFalseLit},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES8[] =
{
    LexicalAnalyzer::TokenValue{ROString("isize"), Token::eISize},
    LexicalAnalyzer::TokenValue{ROString("in"), Token::eIn},
    LexicalAnalyzer::TokenValue{ROString("i64"), Token::eI64},
    LexicalAnalyzer::TokenValue{ROString("i32"), Token::eI32},
    LexicalAnalyzer::TokenValue{ROString("i16"), Token::eI16},
    LexicalAnalyzer::TokenValue{ROString("i8"), Token::eI8},
    LexicalAnalyzer::TokenValue{ROString("if"), Token::eIf},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES17[] =
{
    LexicalAnalyzer::TokenValue{ROString("return"), Token::eReturn},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES18[] =
{
    LexicalAnalyzer::TokenValue{ROString("struct"), Token::eStruct},
    LexicalAnalyzer::TokenValue{ROString("str"), Token::eStr},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES19[] =
{
    LexicalAnalyzer::TokenValue{ROString("type"), Token::eType},
    LexicalAnalyzer::TokenValue{ROString("true"), Token::eTrueLit},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES20[] =
{
    LexicalAnalyzer::TokenValue{ROString("usize"), Token::eUSize},
    LexicalAnalyzer::TokenValue{ROString("u64"), Token::eU64},
    LexicalAnalyzer::TokenValue{ROString("u32"), Token::eU32},
    LexicalAnalyzer::TokenValue{ROString("u16"), Token::eU16},
    LexicalAnalyzer::TokenValue{ROString("u8"), Token::eU8},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES21[] =
{
    LexicalAnalyzer::TokenValue{ROString("var"), Token::eVar},
};

const LexicalAnalyzer::TokenValue KEYWORD_VALUES22[] =
{
    LexicalAnalyzer::TokenValue{ROString("while"), Token::eWhile},
};

const LexicalAnalyzer::Bucket LexicalAnalyzer::KEYWORDS_HASH_TABLE[KEYWORDS_HASH_TABLE_SIZE] =
{
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES1, 2},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES2, 3},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES4, 3},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES5, 3},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES8, 7},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES17, 1},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES18, 2},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES19, 2},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES20, 5},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES21, 1},
    LexicalAnalyzer::Bucket{KEYWORD_VALUES22, 1},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
};

Token::EType LexicalAnalyzer::GetSymbolType(ROString str)
{
    size_t index = static_cast<size_t>(str.GetPtr()[0]) % SYMBOLS_HASH_TABLE_SIZE;
    Bucket bucket = SYMBOLS_HASH_TABLE[index];
    size_t size = bucket.size;
    for (size_t i = 0; i < size; ++i)
    {
        TokenValue value = bucket.values[i];
        if (str == value.str)
        {
            return value.type;
        }
    }

    return Token::eInvalid;
}

Token::EType LexicalAnalyzer::GetKeywordType(ROString str)
{
    size_t index = static_cast<size_t>(str.GetPtr()[0] - 'a');
    if (index < KEYWORDS_HASH_TABLE_SIZE)
    {
        Bucket bucket = KEYWORDS_HASH_TABLE[index];
        size_t size = bucket.size;
        for (size_t i = 0; i < size; ++i)
        {
            TokenValue value = bucket.values[i];
            if (str == value.str)
            {
                return value.type;
            }
        }
    }

    return Token::eInvalid;
}

constexpr bool isWhitespace(char ch)
{
    bool value = ch == ' ';
    value |= ch == '\t';
    value |= ch == '\n';
    value |= ch == '\r';
    return value;
}

LexicalAnalyzer::LexicalAnalyzer(CompilerContext& compilerContext) :
    compilerContext(compilerContext),
    logger(compilerContext.logger)
{
    buffIdx = 0;
    isMore = false;

    filenameId = 0;
}

bool LexicalAnalyzer::Process(const string& inFile)
{
    string filename;
    CharBuffer fileBuff;
    if (inFile.empty() || inFile == "-")
    {
        filename = "<stdin>";

        size_t numChars = 0;
        size_t buffSize = 1024;
        char* buff = new char[buffSize];
        cin.read(buff, buffSize);
        size_t readSize = cin.gcount();
        numChars += readSize;
        while (readSize > 0)
        {
            size_t oldSize = buffSize;
            buffSize *= 2;

            char* oldBuff = buff;
            buff = new char[buffSize];

            memcpy(buff, oldBuff, oldSize);
            delete [] oldBuff;

            cin.read(buff + numChars, buffSize - numChars);
            readSize = cin.gcount();
            numChars += readSize;
        }

        fileBuff.size = numChars;
        fileBuff.ptr = buff;
    }
    else
    {
        filename = inFile;
        fstream file(inFile, ios_base::in | ios_base::binary);
        if (file.fail())
        {
            logger.LogError("Could not open file '{}'", inFile);
            return false;
        }

        fileBuff.size = fs::file_size(inFile);
        char* buff = new char[fileBuff.size];
        file.read(buff, fileBuff.size);
        fileBuff.ptr = buff;
    }

    filenameId = compilerContext.AddFile(filename, fileBuff);
    TokenList& tokens = compilerContext.GetFileTokens(filenameId);

    bool ok = Process(fileBuff, tokens);

    return ok;
}

bool LexicalAnalyzer::Process(CharBuffer buff, TokenList& tokens)
{
    tokens.Clear();
    buffIdx = 0;
    isMore = true;

    line = 1;
    column = 1;

    bool ok = true;
    char ch = Read(buff);
    while (ok && isMore)
    {
        // skip whitespace and comments
        while (isMore && (isWhitespace(ch) || ch == COMMENT_START))
        {
            // skip whitespace
            while (isMore && isWhitespace(ch))
            {
                if (ch == '\n')
                {
                    ++line;
                    column = 1;
                }
                else
                {
                    ++column;
                }

                ch = Read(buff);
            }

            // skip comments
            if (isMore && ch == COMMENT_START)
            {
                ch = Read(buff);
                if (isMore)
                {
                    if (ch == BLOCK_COMMENT_INNER)
                    {
                        ok = ParseBlockComment(buff);
                    }
                    else
                    {
                        ParseLineComment(buff);
                    }
                }

                ch = Read(buff);
            }
        }

        if (isMore)
        {
            const char* valuePtr = buff.ptr + (buffIdx - 1);
            size_t valueSize = 1;

            // parse identifiers and keywords
            if (std::isalpha(ch) || ch == '_')
            {
                unsigned startColumn = column;
                ch = Read(buff);
                ++column;
                while (isMore && (std::isalnum(ch) || ch == '_'))
                {
                    ++valueSize;
                    ch = Read(buff);
                    ++column;
                }

                ROString value(valuePtr, valueSize);

                // get token type
                Token::EType tokenType = GetKeywordType(value);
                if (tokenType == Token::eInvalid)
                {
                    tokenType = Token::eIdentifier;
                }

                tokens.Append(Token(value, filenameId, line, startColumn, tokenType));
            }
            // parse operators and separators
            else if (SYMBOL_START_CHAR.find(ch) != SYMBOL_START_CHAR.end())
            {
                unsigned startColumn = column;

                ch = Read(buff);
                ++column;

                size_t tempSize = 2;
                while (isMore && GetSymbolType(ROString(valuePtr, tempSize)) != Token::eInvalid)
                {
                    ++valueSize;
                    ++tempSize;

                    ch = Read(buff);
                    ++column;
                }

                ROString symbolStr(valuePtr, valueSize);
                Token::EType tokenType = GetSymbolType(symbolStr);
                if (tokenType == Token::eInvalid)
                {
                    logger.LogError(filenameId, line, column, "Invalid symbol");
                    ok = false;
                }
                else
                {
                    tokens.Append(Token(ROString(valuePtr, valueSize), filenameId, line, startColumn, tokenType));
                }
            }
            // parse numeric literals
            else if (ch >= '0' && ch <= '9')
            {
                char firstCh = ch;
                Token::EType tokenType = Token::eDecIntLit;
                unsigned startColumn = column;
                ch = Read(buff);
                ++column;

                if (isMore)
                {
                    if ((ch >= '0' && ch <= '9') || ch == '_' || ch == '.' || ch == 'e')
                    {
                        while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                        {
                            ++valueSize;
                            ch = Read(buff);
                            ++column;
                        }

                        // check if it's a floating-point literal
                        char nextCh = Peek(buff);
                        if (isMore && ( (ch == '.' && nextCh != '.') || ch == 'e') )
                        {
                            tokenType = Token::eFloatLit;

                            if (ch == '.')
                            {
                                ++valueSize;
                                ch = Read(buff);
                                ++column;

                                bool hasDigitAfterPoint = false;
                                while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                                {
                                    if (ch >= '0' && ch <= '9')
                                    {
                                        hasDigitAfterPoint = true;
                                    }

                                    ++valueSize;
                                    ch = Read(buff);
                                    ++column;
                                }

                                if (!hasDigitAfterPoint)
                                {
                                    logger.LogError(filenameId, line, column, "No digit after decimal point");
                                    ok = false;
                                }
                            }

                            if (ok && isMore && ch == 'e')
                            {
                                ++valueSize;
                                ch = Read(buff);
                                ++column;

                                if (isMore && ch == '-')
                                {
                                    ++valueSize;
                                    ch = Read(buff);
                                    ++column;
                                }

                                bool hasDigitAfterE = false;
                                while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                                {
                                    if (ch >= '0' && ch <= '9')
                                    {
                                        hasDigitAfterE = true;
                                    }

                                    ++valueSize;
                                    ch = Read(buff);
                                    ++column;
                                }

                                if (!hasDigitAfterE)
                                {
                                    logger.LogError(filenameId, line, column, "No digit in exponent");
                                    ok = false;
                                }
                            }
                        }

                        if ( ok && ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ) )
                        {
                            logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if (firstCh == '0')
                    {
                        bool hasDigit = false;
                        if (ch == 'x')
                        {
                            tokenType = Token::eHexIntLit;
                            ++valueSize;
                            ch = Read(buff);
                            ++column;

                            while (isMore)
                            {
                                if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
                                {
                                    hasDigit = true;
                                }
                                else if (ch != '_')
                                {
                                    break;
                                }

                                ++valueSize;
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= 'g' && ch <= 'z') || (ch >= 'G' && ch <= 'Z') )
                            {
                                logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filenameId, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if (ch == 'b')
                        {
                            tokenType = Token::eBinIntLit;
                            ++valueSize;
                            ch = Read(buff);
                            ++column;

                            while (isMore)
                            {
                                if (ch == '0' || ch == '1')
                                {
                                    hasDigit = true;
                                }
                                else if (ch != '_')
                                {
                                    break;
                                }

                                ++valueSize;
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= '2' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                            {
                                logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filenameId, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if (ch == 'o')
                        {
                            tokenType = Token::eOctIntLit;
                            ++valueSize;
                            ch = Read(buff);
                            ++column;

                            while (isMore)
                            {
                                if (ch >= '0' && ch <= '7')
                                {
                                    hasDigit = true;
                                }
                                else if (ch != '_')
                                {
                                    break;
                                }

                                ++valueSize;
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= '8' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                            {
                                logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filenameId, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                    {
                        logger.LogError(filenameId, line, column, "Invalid character in numeric literal");
                        ok = false;
                    }
                }

                if (ok)
                {
                    ROString value(valuePtr, valueSize);
                    tokens.Append(Token(value, filenameId, line, startColumn, tokenType));
                }
            }
            // parse string literals
            else if (ch == '"')
            {
                unsigned startColumn = column;
                ch = Read(buff);
                ++column;
                char prevChar = ch;
                while (isMore && (ch != '"' || prevChar == '\\'))
                {
                    if (ch == '\n')
                    {
                        logger.LogError(filenameId, line, column, "Unexpected string end");
                        ok = false;
                        break;
                    }
                    ++valueSize;

                    prevChar = ch;
                    ch = Read(buff);
                    ++column;
                }

                if (!isMore)
                {
                    logger.LogError(filenameId, line, column, "Unexpected string end");
                    ok = false;
                }

                if (ok)
                {
                    ++valueSize; // increment for last '"'
                    ROString value(valuePtr, valueSize);

                    tokens.Append(Token(value, filenameId, line, startColumn, Token::eStrLit));

                    ch = Read(buff);
                    ++column;
                }
            }
            else
            {
                logger.LogError(filenameId, line, column, "Invalid character '{}'", ch);
                ok = false;
            }
        }
    }

    return ok;
}

char LexicalAnalyzer::Read(CharBuffer buff)
{
    char ch;
    isMore = buffIdx < buff.size;
    if (isMore)
    {
        ch = buff.ptr[buffIdx];
        ++buffIdx;
    }
    else
    {
        ch = '\0';
    }

    return ch;
}

char LexicalAnalyzer::Peek(CharBuffer buff)
{
    char ch;
    if (buffIdx < buff.size)
    {
        ch = buff.ptr[buffIdx];
    }
    else
    {
        ch = '\0';
    }

    return ch;
}

void LexicalAnalyzer::ParseLineComment(CharBuffer buff)
{
    char ch = '\0';
    do
    {
        ch = Read(buff);
    } while (isMore && ch != LINE_COMMENT_END);

    ++line;
    column = 1;
}

bool LexicalAnalyzer::ParseBlockComment(CharBuffer buff)
{
    unsigned level = 0;
    char prev = '\0';
    char current = '\0';

    // add 2 for comment start chars
    column += 2;

    current = Read(buff);
    while (isMore)
    {
        if (current == '\n')
        {
            ++line;
            column = 1;
        }
        else
        {
            ++column;
        }

        if (prev == BLOCK_COMMENT_INNER && current == COMMENT_START)
        {
            if (level == 0)
            {
                break;
            }
            else
            {
                --level;
            }
        }
        else if (prev == COMMENT_START && current == BLOCK_COMMENT_INNER)
        {
            ++level;

            // reset previous char to prevent a following COMMENT_START
            // char from ending the comment
            prev = '\0';
        }

        prev = current;
        current = Read(buff);
    }

    return true;
}
