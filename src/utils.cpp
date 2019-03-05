#include "utils.h"

using namespace std;

bool isIdentifier(const string& str)
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

// TODO: This function incorrectly marks strings like "0x" as valid numbers
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
