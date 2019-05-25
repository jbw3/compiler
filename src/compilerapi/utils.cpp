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

bool isBool(const string& tokenStr)
{
    return tokenStr == "true" || tokenStr == "false";
}

bool binStringToInteger(const string& str, int64_t& num)
{
    num = 0;

    // start at 2 to skip leading '0b'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        num <<= 1;

        if (ch == '1')
        {
            num |= 1;
        }
        else if (ch != '0')
        {
            return false;
        }
    }

    return true;
}

bool octStringToInteger(const string& str, int64_t& num)
{
    num = 0;

    // start at 2 to skip leading '0o'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        num *= 8;

        if (ch >= '0' && ch <= '7')
        {
            num += ch - '0';
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool decStringToInteger(const string& str, int64_t& num)
{
    num = 0;

    for (char ch : str)
    {
        if (isdigit(ch))
        {
            num *= 10;
            num += ch - '0';
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool hexStringToInteger(const string& str, int64_t& num)
{
    num = 0;

    // start at 2 to skip leading '0x'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        num *= 16;

        if (isdigit(ch))
        {
            num += ch - '0';
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            num += ch - 'a' + 10;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            num += ch - 'A' + 10;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool stringToInteger(const string& str, int64_t& num)
{
    bool ok = false;

    // determine base
    int base = 10;
    if (str.size() >= 2 && str[0] == '0')
    {
        switch (str[1])
        {
            case 'b':
            case 'B':
                base = 2;
                break;

            case 'o':
            case 'O':
                base = 8;
                break;

            case 'x':
            case 'X':
                base = 16;
                break;

            default:
                base = 10;
                break;
        }
    }

    switch (base)
    {
        case 2:
            ok = binStringToInteger(str, num);
            break;
        case 8:
            ok = octStringToInteger(str, num);
            break;
        case 10:
            ok = decStringToInteger(str, num);
            break;
        case 16:
            ok = hexStringToInteger(str, num);
            break;
        default:
            // we should not get here
            ok = false;
            break;
    }

    return ok;
}

bool stringToBool(const string& str, bool& value)
{
    if (str == "true")
    {
        value = true;
        return true;
    }
    else if (str == "false")
    {
        value = false;
        return true;
    }
    else
    {
        return false;
    }
}
