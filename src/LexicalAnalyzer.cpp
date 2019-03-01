#include "LexicalAnalyzer.h"
#include "Error.h"
#include <iostream>

using namespace std;

constexpr bool isIdentifierChar(char ch, bool isFirstChar)
{
    if (isFirstChar)
    {
        return isalpha(ch) || ch == '_';
    }
    else
    {
        return isalnum(ch) || ch == '_';
    }
}

bool isIdentifier(const std::string& str)
{
    if (str.empty())
    {
        return false;
    }

    if (!isIdentifierChar(str[0], true))
    {
        return false;
    }

    for (size_t i = 1; i < str.size(); ++i)
    {
        if (!isIdentifierChar(str[i], false))
        {
            return false;
        }
    }

    return true;
}

bool isNumber(const string& tokenStr)
{
    size_t idx = 0;

    // determine base
    int base = 10;
    if (tokenStr.size() >= 2 && tokenStr[0] == '0')
    {
        switch (tokenStr[1])
        {
            case 'b':
            case 'B':
                base = 2;
                idx += 2;
                break;

            case 'o':
            case 'O':
                base = 8;
                idx += 2;
                break;

            case 'x':
            case 'X':
                base = 16;
                idx += 2;
                break;

            default:
                base = 10;
                break;
        }
    }

    if (base == 2)
    {
        for (; idx < tokenStr.size(); ++idx)
        {
            if (tokenStr[idx] != '0' && tokenStr[idx] != '1')
            {
                return false;
            }
        }
        return true;
    }
    else if (base == 8)
    {
        for (; idx < tokenStr.size(); ++idx)
        {
            if (tokenStr[idx] < '0' || tokenStr[idx] > '7')
            {
                return false;
            }
        }
        return true;
    }
    else if (base == 10)
    {
        for (; idx < tokenStr.size(); ++idx)
        {
            if (!isdigit(tokenStr[idx]))
            {
                return false;
            }
        }
        return true;
    }
    else if (base == 16)
    {
        for (; idx < tokenStr.size(); ++idx)
        {
            if (!isxdigit(tokenStr[idx]))
            {
                return false;
            }
        }
        return true;
    }

    // should not get here
    return false;
}

LexicalAnalyzer::LexicalAnalyzer() : isValid(false)
{
}

vector<Token> LexicalAnalyzer::Process(istream& is)
{
    vector<Token> tokens;
    tokens.reserve(256);

    tokenStr = "";
    line = 1;
    column = 1;

    char ch = '\0';
    is.read(&ch, 1);
    while (!is.eof())
    {
        ParseChar(ch, tokens);

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

    // check for leftover token
    if (!tokenStr.empty())
    {
        tokens.push_back(CreateToken());
        tokenStr = "";
    }

    return tokens;
}

Token LexicalAnalyzer::CreateToken()
{
    return Token(tokenStr, line, column - tokenStr.size());
}

void LexicalAnalyzer::ParseChar(char ch, std::vector<Token>& tokens)
{
    if (isblank(ch))
    {
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
                ThrowError();
            }
        }
    }
    else // ch is not blank
    {
        if (isValid)
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
            if (ch == '\n')
            {
                // if we reached the end of a line and the last token is
                // not valid, report an error
                if (!tokenStr.empty())
                {
                    ThrowError();
                }
                else
                {
                    ++column;
                    tokenStr = '\n';
                    tokens.push_back(CreateToken());
                    tokenStr = "";
                    isValid = false;
                }
            }
            else
            {
                tokenStr += ch;
                isValid = IsValidToken(tokenStr);
            }
        }

        // if the token string is the end of the line, go ahead
        // and add it before line is incremented
        if (tokenStr == "\n")
        {
            ++column;
            tokens.push_back(CreateToken());
            tokenStr = "";
            isValid = false;
        }
    }
}

bool LexicalAnalyzer::IsValidToken(const string& str) const
{
    return isIdentifier(str) || isNumber(str);
}

void LexicalAnalyzer::ThrowError()
{
    cout << "Invalid syntax: \"" + tokenStr + "\"\n";
    throw Error();
}
