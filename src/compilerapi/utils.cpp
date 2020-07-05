#include "utils.h"
#include <tuple>
#include <vector>

using namespace std;

constexpr char NUMERIC_LITERAL_SEPERATOR = '_';

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

bool isNumber(const string& tokenStr, bool isPotential)
{
    if (tokenStr.size() == 0 || tokenStr[0] == NUMERIC_LITERAL_SEPERATOR)
    {
        return false;
    }

    size_t idx = 0;

    // determine base
    bool sizeCheck = isPotential ? (tokenStr.size() >= 2) : (tokenStr.size() > 2);
    int base = 10;
    if (sizeCheck && tokenStr[0] == '0')
    {
        switch (tokenStr[1])
        {
            case 'b':
                base = 2;
                idx += 2;
                break;

            case 'o':
                base = 8;
                idx += 2;
                break;

            case 'h':
                base = 16;
                idx += 2;
                break;

            default:
                base = 10;
                break;
        }
    }

    vector<tuple<char, char>> ranges;

    if (base == 2)
    {
        ranges.push_back(make_tuple('0', '1'));
    }
    else if (base == 8)
    {
        ranges.push_back(make_tuple('0', '7'));
    }
    else if (base == 10)
    {
        ranges.push_back(make_tuple('0', '9'));
    }
    else if (base == 16)
    {
        ranges.push_back(make_tuple('0', '9'));
        ranges.push_back(make_tuple('a', 'f'));
        ranges.push_back(make_tuple('A', 'F'));
    }
    else
    {
        // should not get here
        return false;
    }

    bool hasDigit = false;
    for (; idx < tokenStr.size(); ++idx)
    {
        char ch = tokenStr[idx];

        if (ch != NUMERIC_LITERAL_SEPERATOR)
        {
            bool found = false;
            for (tuple<char, char> range : ranges)
            {
                if (ch >= get<0>(range) && ch <= get<1>(range))
                {
                    found = true;
                    break;
                }
            }

            // if we could not find the char in any of the valid ranges, return false
            if (!found)
            {
                return false;
            }

            hasDigit = true;
        }
    }

    return hasDigit | isPotential;
}

bool isNumber(const string& tokenStr)
{
    return isNumber(tokenStr, false);
}

bool isPotentialNumber(const string& tokenStr)
{
    return isNumber(tokenStr, true);
}

bool isBool(const string& tokenStr)
{
    return tokenStr == "true" || tokenStr == "false";
}

bool isString(const std::string& tokenStr)
{
    if (tokenStr.size() < 2)
    {
        return false;
    }

    for (size_t i = 1; i < tokenStr.size() - 1; ++i)
    {
        char ch = tokenStr[i];
        if (ch == '"' || !isprint(ch))
        {
            return false;
        }
    }

    if (tokenStr[tokenStr.size() - 1] != '"')
    {
        return false;
    }

    return true;
}

bool isPotentialString(const std::string& tokenStr)
{
    if (tokenStr.empty())
    {
        return false;
    }

    size_t idx = 0;

    if (tokenStr[idx] != '"')
    {
        return false;
    }

    ++idx;
    while (idx < tokenStr.size() - 1)
    {
        char ch = tokenStr[idx];
        if (ch == '"' || !isprint(ch))
        {
            return false;
        }

        ++idx;
    }

    if (idx < tokenStr.size())
    {
        char ch = tokenStr[idx];
        if (ch != '"' && !isprint(ch))
        {
            return false;
        }
    }

    return true;
}

bool binStringToInteger(const string& str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0b'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPERATOR)
        {
            hasDigit = true;

            num <<= 1;

            if (ch == '1')
            {
                num |= 1;
            }
            else if (ch != '0')
            {
                num = 0;
                return false;
            }
        }
    }

    return hasDigit;
}

bool octStringToInteger(const string& str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0o'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPERATOR)
        {
            hasDigit = true;

            num *= 8;

            if (ch >= '0' && ch <= '7')
            {
                num += ch - '0';
            }
            else
            {
                num = 0;
                return false;
            }
        }
    }

    return hasDigit;
}

bool decStringToInteger(const string& str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    for (char ch : str)
    {
        if (ch != NUMERIC_LITERAL_SEPERATOR)
        {
            hasDigit = true;

            if (isdigit(ch))
            {
                num *= 10;
                num += ch - '0';
            }
            else
            {
                num = 0;
                return false;
            }
        }
    }

    return hasDigit;
}

bool hexStringToInteger(const string& str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0x'
    for (size_t i = 2; i < str.size(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPERATOR)
        {
            hasDigit = true;

            num *= 16;

            char digitNum = '\0';
            if (hexDigitToNum(ch, digitNum))
            {
                num += digitNum;
            }
            else
            {
                num = 0;
                return false;
            }
        }
    }

    return hasDigit;
}

bool stringToInteger(const string& str, int64_t& num)
{
    bool ok = false;

    if (str.size() == 0 || str[0] == NUMERIC_LITERAL_SEPERATOR)
    {
        num = 0;
        ok = false;
    }
    else
    {
        // determine base
        int base = 10;
        if (str.size() > 2 && str[0] == '0')
        {
            switch (str[1])
            {
                case 'b':
                    base = 2;
                    break;

                case 'o':
                    base = 8;
                    break;

                case 'h':
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
                num = 0;
                ok = false;
                break;
        }
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
