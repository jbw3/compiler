#include "LexicalAnalyzer.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

const char LexicalAnalyzer::COMMENT_START = '#';

const char LexicalAnalyzer::COMMENT_END = '\n';

const unordered_set<string> LexicalAnalyzer::SYMBOLS =
{
    "==", "!=", "<", "<=", ">", ">=", "+", "-", "*", "/", "%", "<<", ">>", "!", "&", "^", "|", "&&", "||", "(", ")", "{", "}", ",",
    ";", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=",
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
        is = &cin;
    }
    else
    {
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

bool LexicalAnalyzer::Process(std::istream& is, std::vector<Token>& tokens)
{
    tokens.clear();
    tokens.reserve(256);

    tokenStr = "";
    line = 1;
    column = 1;

    bool ok = true;
    char ch = '\0';
    is.read(&ch, 1);
    while (!is.eof())
    {
        if (ch == COMMENT_START && !isString)
        {
            // read to comment end
            do
            {
                is.read(&ch, 1);
            } while (!is.eof() && ch != COMMENT_END);
        }
        else
        {
            ok = ParseChar(ch, tokens);
            if (!ok)
            {
                break;
            }
        }

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
    if (ok && !tokenStr.empty())
    {
        tokens.push_back(CreateToken());
        tokenStr = "";
    }

    return ok;
}

unsigned long LexicalAnalyzer::GetTokenStrStartColumn()
{
    return column - tokenStr.size();
}

Token LexicalAnalyzer::CreateToken()
{
    return Token(tokenStr, line, GetTokenStrStartColumn());
}

bool LexicalAnalyzer::ParseChar(char ch, std::vector<Token>& tokens)
{
    if (isString)
    {
        if (ch == '"' && tokenStr.back() != '\\')
        {
            tokenStr += ch;
            tokens.push_back(CreateToken());
            tokenStr = "";
            isValid = false;
            isString = false;
        }
        else if (ch == '\n')
        {
            logger.LogError(line, column, "Unexpected string end");
            return false;
        }
        else
        {
            tokenStr += ch;
        }
    }
    else if (isspace(ch))
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
                PrintError();
                return false;
            }
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

void LexicalAnalyzer::PrintError()
{
    logger.LogError(line, GetTokenStrStartColumn(), "Invalid syntax: '{}'", tokenStr);
}
