#include "LexicalAnalyzer.h"
#include "utils.h"
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

const unordered_set<string> LexicalAnalyzer::SYMBOLS =
{
    "==", "!=", "<", "<=", ">", ">=", "+", "-", "*", "/", "%", "<<", ">>", ">>>", "!", "&", "^", "|", "&&", "||", "(", ")", "[", "]", "{", "}", ",",
    ".", ";", ":", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", ">>>=", "&=", "^=", "|=", "..", "..<",
};

constexpr bool isWhitespace(char ch)
{
    bool value = ch == ' ';
    value |= ch == '\t';
    value |= ch == '\n';
    value |= ch == '\r';
    return value;
}

LexicalAnalyzer::LexicalAnalyzer(ErrorLogger& logger) :
    logger(logger)
{
    buff = new char[MAX_BUFF_CAPACITY];
    buffSize = 0;
    buffIdx = 0;
    isMore = false;
}

LexicalAnalyzer::~LexicalAnalyzer()
{
    delete [] buff;
}

bool LexicalAnalyzer::Process(const string& inFile, vector<Token>& tokens)
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

    bool ok = Process(*is, tokens);

    if (is != &cin)
    {
        delete is;
    }

    return ok;
}

bool LexicalAnalyzer::Process(istream& is, vector<Token>& tokens)
{
    tokens.clear();
    tokens.reserve(256);
    buffSize = 0;
    buffIdx = 0;
    isMore = true;

    tokenStr = "";
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
            if (isIdentifierChar(ch, true))
            {
                unsigned startColumn = column;
                tokenStr += ch;
                ch = Read(is);
                ++column;
                while (isMore && isIdentifierChar(ch, false))
                {
                    tokenStr += ch;
                    ch = Read(is);
                    ++column;
                }

                tokens.push_back(Token(tokenStr, filename, line, startColumn));
                tokenStr.clear();
            }
            // parse operators and separators
            else if (SYMBOL_START_CHAR.find(ch) != SYMBOL_START_CHAR.end())
            {
                unsigned startColumn = column;
                tokenStr += ch;
                ch = Read(is);
                ++column;
                while (isMore && SYMBOLS.find(tokenStr + ch) != SYMBOLS.end())
                {
                    tokenStr += ch;
                    ch = Read(is);
                    ++column;
                }

                tokens.push_back(Token(tokenStr, filename, line, startColumn));
                tokenStr.clear();
            }
            // parse numeric literals
            else if (ch >= '0' && ch <= '9')
            {
                unsigned startColumn = column;
                tokenStr += ch;
                ch = Read(is);
                ++column;

                if ( isMore && ((ch >= '0' && ch <= '9') || ch == '_' || ch == 'x' || ch == 'b' || ch == 'o') )
                {
                    if ((ch >= '0' && ch <= '9') || ch == '_')
                    {
                        while ( isMore && ((ch >= '0' && ch <= '9') || ch == '_') )
                        {
                            tokenStr += ch;
                            ch = Read(is);
                            ++column;
                        }

                        if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if (ch == 'x')
                    {
                        tokenStr += ch;
                        ch = Read(is);
                        ++column;

                        while ( isMore && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || ch == '_') )
                        {
                            tokenStr += ch;
                            ch = Read(is);
                            ++column;
                        }

                        if ( (ch >= 'g' && ch <= 'z') || (ch >= 'G' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if (ch == 'b')
                    {
                        tokenStr += ch;
                        ch = Read(is);
                        ++column;

                        while ( isMore && (ch == '0' || ch == '1' || ch == '_') )
                        {
                            tokenStr += ch;
                            ch = Read(is);
                            ++column;
                        }

                        if ( (ch >= '2' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else if (ch == 'o')
                    {
                        tokenStr += ch;
                        ch = Read(is);
                        ++column;

                        while ( isMore && ((ch >= '0' && ch <= '7') || ch == '_') )
                        {
                            tokenStr += ch;
                            ch = Read(is);
                            ++column;
                        }

                        if ( (ch >= '8' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') )
                        {
                            logger.LogError(filename, line, column, "Invalid character in numeric literal");
                            ok = false;
                        }
                    }
                    else
                    {
                        logger.LogError(filename, line, column, "Invalid character in numeric literal");
                        ok = false;
                    }
                }

                if (ok)
                {
                    tokens.push_back(Token(tokenStr, filename, line, startColumn));
                    tokenStr.clear();
                }
            }
            // parse string literals
            else if (ch == '"')
            {
                unsigned startColumn = column;
                tokenStr += ch;
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
                    tokenStr += ch;

                    prevChar = ch;
                    ch = Read(is);
                    ++column;
                }

                if (ok)
                {
                    tokenStr += ch; // add last '"'
                    tokens.push_back(Token(tokenStr, filename, line, startColumn));
                    tokenStr.clear();
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
