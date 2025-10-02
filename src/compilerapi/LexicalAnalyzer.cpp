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

const LexicalAnalyzer::TokenValue SYMBOL_VALUES1[] =
{
    LexicalAnalyzer::TokenValue{ROString("%"), Token::eRemainder},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES2[] =
{
    LexicalAnalyzer::TokenValue{ROString("&"), Token::eAmpersand},
    LexicalAnalyzer::TokenValue{ROString("*="), Token::eTimesEqual},
    LexicalAnalyzer::TokenValue{ROString("..<"), Token::ePeriodPeriodLess},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES3[] =
{
    LexicalAnalyzer::TokenValue{ROString("+="), Token::ePlusEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES5[] =
{
    LexicalAnalyzer::TokenValue{ROString("!"), Token::eExclaim},
    LexicalAnalyzer::TokenValue{ROString("-="), Token::eMinusEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES6[] =
{
    LexicalAnalyzer::TokenValue{ROString(".."), Token::ePeriodPeriod},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES7[] =
{
    LexicalAnalyzer::TokenValue{ROString("/="), Token::eDivideEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES8[] =
{
    LexicalAnalyzer::TokenValue{ROString(","), Token::eComma},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES9[] =
{
    LexicalAnalyzer::TokenValue{ROString("-"), Token::eMinus},
    LexicalAnalyzer::TokenValue{ROString("!="), Token::eExclaimEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES10[] =
{
    LexicalAnalyzer::TokenValue{ROString("."), Token::ePeriod},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES11[] =
{
    LexicalAnalyzer::TokenValue{ROString("/"), Token::eDivide},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES12[] =
{
    LexicalAnalyzer::TokenValue{ROString("("), Token::eOpenPar},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES13[] =
{
    LexicalAnalyzer::TokenValue{ROString(")"), Token::eClosePar},
    LexicalAnalyzer::TokenValue{ROString("%="), Token::eRemainderEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES14[] =
{
    LexicalAnalyzer::TokenValue{ROString("*"), Token::eTimes},
    LexicalAnalyzer::TokenValue{ROString("&&"), Token::eAmpersandAmpersand},
    LexicalAnalyzer::TokenValue{ROString("&="), Token::eAmpersandEqual},
    LexicalAnalyzer::TokenValue{ROString(">>>="), Token::eGreaterGreaterGreaterEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES15[] =
{
    LexicalAnalyzer::TokenValue{ROString("+"), Token::ePlus},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES16[] =
{
    LexicalAnalyzer::TokenValue{ROString("<<="), Token::eLessLessEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES18[] =
{
    LexicalAnalyzer::TokenValue{ROString(">>="), Token::eGreaterGreaterEqual},
    LexicalAnalyzer::TokenValue{ROString(">>>"), Token::eGreaterGreaterGreater},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES20[] =
{
    LexicalAnalyzer::TokenValue{ROString("<="), Token::eLessEqual},
    LexicalAnalyzer::TokenValue{ROString("||"), Token::eBarBar},
    LexicalAnalyzer::TokenValue{ROString("<<"), Token::eLessLess},
    LexicalAnalyzer::TokenValue{ROString("|="), Token::eBarEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES21[] =
{
    LexicalAnalyzer::TokenValue{ROString("=="), Token::eEqualEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES22[] =
{
    LexicalAnalyzer::TokenValue{ROString(">="), Token::eGreaterEqual},
    LexicalAnalyzer::TokenValue{ROString(">>"), Token::eGreaterGreater},
    LexicalAnalyzer::TokenValue{ROString("^="), Token::eCaretEqual},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES24[] =
{
    LexicalAnalyzer::TokenValue{ROString("<"), Token::eLess},
    LexicalAnalyzer::TokenValue{ROString("|"), Token::eBar},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES25[] =
{
    LexicalAnalyzer::TokenValue{ROString("="), Token::eEqual},
    LexicalAnalyzer::TokenValue{ROString("}"), Token::eCloseBrace},
    LexicalAnalyzer::TokenValue{ROString("]"), Token::eCloseBracket},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES26[] =
{
    LexicalAnalyzer::TokenValue{ROString(">"), Token::eGreater},
    LexicalAnalyzer::TokenValue{ROString("^"), Token::eCaret},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES30[] =
{
    LexicalAnalyzer::TokenValue{ROString(":"), Token::eColon},
};

const LexicalAnalyzer::TokenValue SYMBOL_VALUES31[] =
{
    LexicalAnalyzer::TokenValue{ROString(";"), Token::eSemiColon},
    LexicalAnalyzer::TokenValue{ROString("{"), Token::eOpenBrace},
    LexicalAnalyzer::TokenValue{ROString("["), Token::eOpenBracket},
};

const LexicalAnalyzer::Bucket LexicalAnalyzer::SYMBOLS_HASH_TABLE[SYMBOLS_HASH_TABLE_SIZE] =
{
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES1, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES2, 3},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES3, 1},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES5, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES6, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES7, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES8, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES9, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES10, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES11, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES12, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES13, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES14, 4},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES15, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES16, 1},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES18, 2},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES20, 4},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES21, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES22, 3},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES24, 2},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES25, 3},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES26, 2},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{nullptr, 0},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES30, 1},
    LexicalAnalyzer::Bucket{SYMBOL_VALUES31, 3},
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
    size_t index = static_cast<size_t>(str[0]) ^ (str.GetSize() << 2);
    index %= SYMBOLS_HASH_TABLE_SIZE;

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
    size_t index = static_cast<size_t>(str[0] - 'a');
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
                bool escaping = false;
                while (isMore && (ch != '"' || escaping))
                {
                    if (ch == '\n')
                    {
                        logger.LogError(filenameId, line, column, "Unexpected string end");
                        ok = false;
                        break;
                    }

                    if (escaping)
                    {
                        escaping = false;
                    }
                    else if (ch == '\\')
                    {
                        escaping = true;
                    }

                    ++valueSize;

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
            // parse built-in identifiers
            else if (ch == '@')
            {
                unsigned startColumn = column;
                ch = Read(buff);
                ++column;

                if (!isMore || (!std::isalpha(ch) && ch != '_'))
                {
                    logger.LogError(filenameId, line, column, "Expected a letter or underscore after '@'");
                    ok = false;
                }
                else
                {
                    while (isMore && (std::isalnum(ch) || ch == '_'))
                    {
                        ++valueSize;
                        ch = Read(buff);
                        ++column;
                    }

                    ROString value(valuePtr, valueSize);
                    tokens.Append(Token(value, filenameId, line, startColumn, Token::eBuiltInIdentifier));
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
