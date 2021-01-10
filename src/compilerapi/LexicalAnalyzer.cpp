#include "LexicalAnalyzer.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

const char LexicalAnalyzer::COMMENT_START = '#';

const char LexicalAnalyzer::BLOCK_COMMENT_INNER = '!';

const char LexicalAnalyzer::LINE_COMMENT_END = '\n';

const unordered_set<string> LexicalAnalyzer::SYMBOLS =
{
    "==", "!=", "<", "<=", ">", ">=", "+", "-", "*", "/", "%", "<<", ">>", ">>>", "!", "&", "^", "|", "&&", "||", "(", ")", "[", "]", "{", "}", ",",
    ".", ";", ":", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", ">>>=", "&=", "^=", "|=", "..", "..<",
};

LexicalAnalyzer::LexicalAnalyzer(ErrorLogger& logger) :
    logger(logger),
    isValid(false),
    isString(false)
{
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

    tokenStr = "";
    isValid = false;
    isString = false;
    line = 1;
    column = 1;

    bool ok = true;
    char ch = '\0';
    is.read(&ch, 1);
    while (ok && !is.eof())
    {
        // skip whitespace and comments
        while (!is.eof() && (isspace(ch) || ch == COMMENT_START))
        {
            // skip whitespace
            while (!is.eof() && isspace(ch))
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

                is.read(&ch, 1);
            }

            // skip comments
            if (!is.eof() && ch == COMMENT_START)
            {
                is.read(&ch, 1);
                if (!is.eof())
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

                is.read(&ch, 1);
            }
        }

        if (!is.eof())
        {
            // parse identifiers and keywords
            if (isIdentifierChar(ch, true))
            {
                unsigned startColumn = column;
                tokenStr += ch;
                is.read(&ch, 1);
                ++column;
                while (!is.eof() && isIdentifierChar(ch, false))
                {
                    tokenStr += ch;
                    is.read(&ch, 1);
                    ++column;
                }

                tokens.push_back(Token(tokenStr, filename, line, startColumn));
                tokenStr.clear();
            }
            // parse operators and separators
            else if (false)
            {
                // TODO
            }
            // parse int literals
            else if (ch >= '0' && ch <= '9')
            {
                // TODO
            }
            // parse string literals
            else if (ch == '"')
            {
                unsigned startColumn = column;
                tokenStr += ch;
                is.read(&ch, 1);
                ++column;
                char prevChar = ch;
                while (!is.eof() && (ch != '"' || prevChar == '\\'))
                {
                    if (ch == '\n')
                    {
                        logger.LogError(filename, line, column, "Unexpected string end");
                        ok = false;
                        break;
                    }
                    tokenStr += ch;

                    prevChar = ch;
                    is.read(&ch, 1);
                    ++column;
                }

                if (ok)
                {
                    tokenStr += ch; // add last '"'
                    tokens.push_back(Token(tokenStr, filename, line, startColumn));
                    tokenStr.clear();
                    is.read(&ch, 1);
                }
            }
            else
            {
                logger.LogError(filename, line, column, "Invalid character '{}'", ch);
                ok = false;
            }
        }

#if false
        if (ch == COMMENT_START && !isString)
        {
            ok = AddTokenIfValid(tokens);

            if (ok)
            {
                is.read(&ch, 1);
                if (!is.eof())
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
            }
        }
        else
        {
            ok = ParseChar(ch, tokens);
            if (ch == '\n')
            {
                ++line;
                column = 1;
            }
            else
            {
                ++column;
            }
        }

        if (!ok)
        {
            break;
        }
#endif
    }

#if false
    // check for leftover token
    if (ok && !tokenStr.empty())
    {
        tokens.push_back(CreateToken());
        tokenStr = "";
    }
#endif

    return ok;
}

unsigned LexicalAnalyzer::GetTokenStrStartColumn()
{
    return column - tokenStr.size();
}

Token LexicalAnalyzer::CreateToken(unsigned columnNum)
{
    if (columnNum == 0)
    {
        columnNum = GetTokenStrStartColumn();
    }

    return Token(tokenStr, filename, line, columnNum);
}

bool LexicalAnalyzer::ParseChar(char ch, vector<Token>& tokens)
{
    if (isString)
    {
        if (ch == '"' && tokenStr.back() != '\\')
        {
            tokenStr += ch;
            tokens.push_back(CreateToken(column - tokenStr.size() + 1));
            tokenStr = "";
            isValid = false;
            isString = false;
        }
        else if (ch == '\n')
        {
            logger.LogError(filename, line, column, "Unexpected string end");
            return false;
        }
        else
        {
            tokenStr += ch;
        }
    }
    else if (isspace(ch))
    {
        bool ok = AddTokenIfValid(tokens);
        if (!ok)
        {
            return false;
        }
    }
    else // we're not in a string and ch is not whitespace
    {
        if (ch == '"')
        {
            if (!tokenStr.empty())
            {
                if (isValid)
                {
                    tokens.push_back(CreateToken());
                }
                else
                {
                    PrintError();
                    return false;
                }
            }

            tokenStr = ch;
            isValid = false;
            isString = true;
        }
        else if (isValid)
        {
            if (IsValidToken(tokenStr + ch))
            {
                tokenStr += ch;
            }
            else
            {
                tokens.push_back(CreateToken());
                tokenStr = ch;
                isValid = IsValidToken(tokenStr);
            }
        }
        else
        {
            tokenStr += ch;
            isValid = IsValidToken(tokenStr);
        }
    }

    return true;
}

bool LexicalAnalyzer::IsValidToken(const string& str) const
{
    return SYMBOLS.find(str) != SYMBOLS.end() || isIdentifier(str) || isPotentialNumber(str);
}

bool LexicalAnalyzer::AddTokenIfValid(vector<Token>& tokens)
{
    bool ok = true;

    if (!tokenStr.empty())
    {
        if (isValid)
        {
            tokens.push_back(CreateToken());
            tokenStr = "";
            isValid = false;
        }
        else
        {
            PrintError();
            ok = false;
        }
    }

    return ok;
}

void LexicalAnalyzer::ParseLineComment(istream& is)
{
    char ch = '\0';
    do
    {
        is.read(&ch, 1);
    } while (!is.eof() && ch != LINE_COMMENT_END);

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

    is.read(&current, 1);
    while (!is.eof())
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
        is.read(&current, 1);
    }

    return true;
}

void LexicalAnalyzer::PrintError()
{
    logger.LogError(filename, line, GetTokenStrStartColumn(), "Invalid syntax: '{}'", tokenStr);
}
