#include "LexicalAnalyzer.h"
#include "CompilerContext.h"
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

const unordered_map<const char*, Token::EType, CStringHash, CStringEqual> LexicalAnalyzer::SYMBOLS =
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

const unordered_map<const char*, Token::EType, CStringHash, CStringEqual> LexicalAnalyzer::KEYWORDS =
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
    { UINT8_KEYWORD, Token::eU8 },
    { UINT16_KEYWORD, Token::eU16 },
    { UINT32_KEYWORD, Token::eU32 },
    { UINT64_KEYWORD, Token::eU64 },
    { UINT_SIZE_KEYWORD, Token::eUSize },
    { VARIABLE_KEYWORD, Token::eVar },
    { WHILE_KEYWORD, Token::eWhile },
};

constexpr bool isWhitespace(char ch)
{
    bool value = ch == ' ';
    value |= ch == '\t';
    value |= ch == '\n';
    value |= ch == '\r';
    return value;
}

LexicalAnalyzer::LexicalAnalyzer(CompilerContext& compilerContext, ErrorLogger& logger) :
    compilerContext(compilerContext),
    logger(logger)
{
    buffIdx = 0;
    isMore = false;

    filenameId = 0;
}

bool LexicalAnalyzer::Process(const string& inFile)
{
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
        fstream file(inFile, ios_base::in);
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
    TokenValues& tokenValues = compilerContext.tokenValues;
    tokenValues.ClearCurrent();
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
            // parse identifiers and keywords
            if (std::isalpha(ch) || ch == '_')
            {
                unsigned startColumn = column;
                tokenValues.AppendChar(ch);
                ch = Read(buff);
                ++column;
                while (isMore && (std::isalnum(ch) || ch == '_'))
                {
                    tokenValues.AppendChar(ch);
                    ch = Read(buff);
                    ++column;
                }

                const char* value = tokenValues.EndValue();

                // get token type
                Token::EType tokenType = Token::eInvalid;
                auto iter = KEYWORDS.find(value);
                if (iter != KEYWORDS.end())
                {
                    value = iter->first;
                    tokenType = iter->second;

                    tokenValues.ClearCurrent();
                }
                else
                {
                    tokenType = Token::eIdentifier;

                    tokenValues.StartNew();
                }

                tokens.Append(Token(value, filenameId, line, startColumn, tokenType));
            }
            // parse operators and separators
            else if (SYMBOL_START_CHAR.find(ch) != SYMBOL_START_CHAR.end())
            {
                unsigned startColumn = column;
                tokenValues.AppendChar(ch);

                char temp[8];
                size_t tempIdx = 0;
                temp[tempIdx++] = ch;

                ch = Read(buff);
                ++column;

                temp[tempIdx++] = ch;
                temp[tempIdx] = '\0';
                while (isMore && SYMBOLS.find(temp) != SYMBOLS.end())
                {
                    tokenValues.AppendChar(ch);
                    ch = Read(buff);
                    ++column;

                    temp[tempIdx++] = ch;
                    temp[tempIdx] = '\0';
                }

                const char* symbolValue = tokenValues.EndValue();

                auto iter = SYMBOLS.find(symbolValue);
                if (iter == SYMBOLS.end())
                {
                    logger.LogError(filename, line, column, "Invalid symbol");
                    ok = false;
                }
                else
                {
                    const char* value = iter->first;
                    Token::EType tokenType = iter->second;

                    tokens.Append(Token(value, filenameId, line, startColumn, tokenType));

                    tokenValues.ClearCurrent();
                }
            }
            // parse numeric literals
            else if (ch >= '0' && ch <= '9')
            {
                char firstCh = ch;
                Token::EType tokenType = Token::eDecIntLit;
                unsigned startColumn = column;
                tokenValues.AppendChar(ch);
                ch = Read(buff);
                ++column;

                if (isMore)
                {
                    if ((ch >= '0' && ch <= '9') || ch == '_')
                    {
                        while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                        {
                            tokenValues.AppendChar(ch);
                            ch = Read(buff);
                            ++column;
                        }

                        if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if (firstCh == '0')
                    {
                        bool hasDigit = false;
                        if (ch == 'x')
                        {
                            tokenType = Token::eHexIntLit;
                            tokenValues.AppendChar(ch);
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

                                tokenValues.AppendChar(ch);
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= 'g' && ch <= 'z') || (ch >= 'G' && ch <= 'Z') )
                            {
                                logger.LogError(filename, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filename, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if (ch == 'b')
                        {
                            tokenType = Token::eBinIntLit;
                            tokenValues.AppendChar(ch);
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

                                tokenValues.AppendChar(ch);
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= '2' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                            {
                                logger.LogError(filename, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filename, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if (ch == 'o')
                        {
                            tokenType = Token::eOctIntLit;
                            tokenValues.AppendChar(ch);
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

                                tokenValues.AppendChar(ch);
                                ch = Read(buff);
                                ++column;
                            }

                            if ( (ch >= '8' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                            {
                                logger.LogError(filename, line, column, "Invalid character in numeric literal");
                                ok = false;
                            }
                            else if (!hasDigit)
                            {
                                logger.LogError(filename, line, column - 1, "Numeric literal ended without a digit");
                                ok = false;
                            }
                        }
                        else if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                    {
                        logger.LogError(filename, line, column, "Invalid character in numeric literal");
                        ok = false;
                    }
                }

                if (ok)
                {
                    const char* value = tokenValues.EndValue();
                    tokens.Append(Token(value, filenameId, line, startColumn, tokenType));
                    tokenValues.StartNew();
                }
            }
            // parse string literals
            else if (ch == '"')
            {
                unsigned startColumn = column;
                tokenValues.AppendChar(ch);
                ch = Read(buff);
                ++column;
                char prevChar = ch;
                while (isMore && (ch != '"' || prevChar == '\\'))
                {
                    if (ch == '\n')
                    {
                        logger.LogError(filename, line, column, "Unexpected string end");
                        ok = false;
                        break;
                    }
                    tokenValues.AppendChar(ch);

                    prevChar = ch;
                    ch = Read(buff);
                    ++column;
                }

                if (!isMore)
                {
                    logger.LogError(filename, line, column, "Unexpected string end");
                    ok = false;
                }

                if (ok)
                {
                    tokenValues.AppendChar(ch); // add last '"'
                    const char* value = tokenValues.EndValue();

                    tokens.Append(Token(value, filenameId, line, startColumn, Token::eStrLit));
                    tokenValues.StartNew();

                    ch = Read(buff);
                    ++column;
                }
            }
            else
            {
                logger.LogError(filename, line, column, "Invalid character '{}'", ch);
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
