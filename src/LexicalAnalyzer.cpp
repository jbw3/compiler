#include "LexicalAnalyzer.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

const unordered_set<string> LexicalAnalyzer::SYMBOLS = {"+", "-", "(", ")", ",", ";"};

LexicalAnalyzer::LexicalAnalyzer() : isValid(false)
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
            cerr << "Error: Could not open file \"" << inFile << "\"\n";
            return false;
        }
    }

    tokens.clear();
    tokens.reserve(256);

    tokenStr = "";
    line = 1;
    column = 1;

    bool ok = true;
    char ch = '\0';
    is->read(&ch, 1);
    while (!is->eof())
    {
        ok = ParseChar(ch, tokens);
        if (!ok)
        {
            break;
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

        is->read(&ch, 1);
    }

    // check for leftover token
    if (ok && !tokenStr.empty())
    {
        tokens.push_back(CreateToken());
        tokenStr = "";
    }

    if (is != &cin)
    {
        delete is;
    }

    return ok;
}

Token LexicalAnalyzer::CreateToken()
{
    return Token(tokenStr, line, column - tokenStr.size());
}

bool LexicalAnalyzer::ParseChar(char ch, std::vector<Token>& tokens)
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
                PrintError();
                return false;
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
                    PrintError();
                    return false;
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

    return true;
}

bool LexicalAnalyzer::IsValidToken(const string& str) const
{
    return SYMBOLS.find(str) != SYMBOLS.end() || isIdentifier(str) || isNumber(str);
}

void LexicalAnalyzer::PrintError()
{
    cerr << "Invalid syntax: \"" + tokenStr + "\"\n";
}
