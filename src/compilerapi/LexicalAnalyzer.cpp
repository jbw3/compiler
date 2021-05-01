#include "LexicalAnalyzer.h"
#include "CompilerContext.h"
#include "keywords.h"
#include <fstream>
#include <iostream>

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
    buff = new char[MAX_BUFF_CAPACITY];
    buffSize = 0;
    buffIdx = 0;
    isMore = false;

    filenameId = 0;
}

LexicalAnalyzer::~LexicalAnalyzer()
{
    delete [] buff;
}

bool LexicalAnalyzer::Process(const string& inFile)
{
    istream* is = nullptr;
    if (inFile.empty() || inFile == "-")
    {
        filename = "<stdin>";
        is = &cin;
    }
    else
    {
        filename = inFile;
        is = new fstream(inFile, ios_base::in);
        if (is->fail())
        {
            delete is;
            logger.LogError("Could not open file '{}'", inFile);
            return false;
        }
    }

    filenameId = compilerContext.AddFile(filename);
    TokenList& tokens = compilerContext.GetFileTokens(filenameId);

    bool ok = Process(*is, tokens);

    if (is != &cin)
    {
        delete is;
    }

    return ok;
}

bool LexicalAnalyzer::Process(istream& is, TokenList& tokens)
{
    TokenValues& tokenValues = compilerContext.tokenValues;
    tokenValues.ClearCurrent();
    tokens.Clear();
    buffSize = 0;
    buffIdx = 0;
    isMore = true;

    line = 1;
    column = 1;

    bool ok = true;
    char ch = Read(is);
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

                ch = Read(is);
            }

            // skip comments
            if (isMore && ch == COMMENT_START)
            {
                ch = Read(is);
                if (isMore)
                {
                    if (ch == BLOCK_COMMENT_INNER)
                    {
                        ok = ParseBlockComment(is);
                    }
                    else
                    {
                        ParseLineComment(is);
                    }
                }

                ch = Read(is);
            }
        }

        if (isMore)
        {
            // parse identifiers and keywords
            if (std::isalpha(ch) || ch == '_')
            {
                unsigned startColumn = column;
                tokenValues.AppendChar(ch);
                ch = Read(is);
                ++column;
                while (isMore && (std::isalnum(ch) || ch == '_'))
                {
                    tokenValues.AppendChar(ch);
                    ch = Read(is);
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

                ch = Read(is);
                ++column;

                temp[tempIdx++] = ch;
                temp[tempIdx] = '\0';
                while (isMore && SYMBOLS.find(temp) != SYMBOLS.end())
                {
                    tokenValues.AppendChar(ch);
                    ch = Read(is);
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
                ch = Read(is);
                ++column;

                if (isMore)
                {
                    if ((ch >= '0' && ch <= '9') || ch == '_')
                    {
                        while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                        {
                            tokenValues.AppendChar(ch);
                            ch = Read(is);
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
                            ch = Read(is);
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
                                ch = Read(is);
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
                            ch = Read(is);
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
                                ch = Read(is);
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
                            ch = Read(is);
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
                                ch = Read(is);
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
                ch = Read(is);
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
                    ch = Read(is);
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

                    ch = Read(is);
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

char LexicalAnalyzer::Read(istream& is)
{
    char ch;
    if (buffIdx < buffSize)
    {
        ch = buff[buffIdx];
        ++buffIdx;
    }
    else
    {
        is.read(buff, MAX_BUFF_CAPACITY);
        buffSize = is.gcount();
        if (buffSize == 0)
        {
            isMore = false;
            ch = '\0';
        }
        else
        {
            ch = buff[0];
            buffIdx = 1;
        }
    }

    return ch;
}

void LexicalAnalyzer::ParseLineComment(istream& is)
{
    char ch = '\0';
    do
    {
        ch = Read(is);
    } while (isMore && ch != LINE_COMMENT_END);

    ++line;
    column = 1;
}

bool LexicalAnalyzer::ParseBlockComment(istream& is)
{
    unsigned level = 0;
    char prev = '\0';
    char current = '\0';

    // add 2 for comment start chars
    column += 2;

    current = Read(is);
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
        current = Read(is);
    }

    return true;
}
