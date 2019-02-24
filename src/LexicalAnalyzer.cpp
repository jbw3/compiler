#include "LexicalAnalyzer.h"

using namespace std;

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
        if (isblank(ch))
        {
            if (!tokenStr.empty())
            {
                tokens.push_back(CreateToken());
                tokenStr = "";
            }
        }
        else
        {
            tokenStr += ch;
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
