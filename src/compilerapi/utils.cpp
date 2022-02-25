#include "utils.h"
#include <tuple>
#include <vector>

using namespace std;

constexpr char NUMERIC_LITERAL_SEPERATOR = '_';

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

                case 'x':
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

double stringToFloat(const string& str)
{
    size_t strSize = str.size();
    size_t idx = 0;

    // process number before decimal point
    double wholePart = 0.0;
    while (idx < strSize)
    {
        char ch = str[idx];

        if (ch == '.')
        {
            break;
        }
        else if (ch != '_')
        {
            wholePart *= 10.0;
            wholePart += ch - '0';
        }

        ++idx;
    }

    // process number after decimal point
    double fractionalPart = 0.0;
    if (idx < strSize)
    {
        idx = strSize - 1; // reset index to last char in string
        while (idx > 0)
        {
            char ch = str[idx];

            if (ch == '.')
            {
                break;
            }
            else if (ch != '_')
            {
                fractionalPart += ch - '0';
                fractionalPart /= 10.0;
            }

            ++idx;
        }
    }

    double num = wholePart + fractionalPart;
    return num;
}
