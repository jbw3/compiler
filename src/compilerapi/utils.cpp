#include "utils.h"
#include <cmath>

using namespace std;

constexpr char NUMERIC_LITERAL_SEPARATOR = '_';

bool binStringToInteger(ROString str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0b'
    for (size_t i = 2; i < str.GetSize(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPARATOR)
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

bool octStringToInteger(ROString str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0o'
    for (size_t i = 2; i < str.GetSize(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPARATOR)
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

bool decStringToInteger(ROString str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    for (size_t i = 0; i < str.GetSize(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPARATOR)
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

bool hexStringToInteger(ROString str, int64_t& num)
{
    num = 0;
    bool hasDigit = false;

    // start at 2 to skip leading '0x'
    for (size_t i = 2; i < str.GetSize(); ++i)
    {
        char ch = str[i];
        if (ch != NUMERIC_LITERAL_SEPARATOR)
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

bool stringToInteger(ROString str, int64_t& num)
{
    bool ok = false;

    if (str.GetSize() == 0 || str[0] == NUMERIC_LITERAL_SEPARATOR)
    {
        num = 0;
        ok = false;
    }
    else
    {
        // determine base
        int base = 10;
        if (str.GetSize() > 2 && str[0] == '0')
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

double stringToFloat(ROString str)
{
    size_t strSize = str.GetSize();
    size_t fractionStartIdx = strSize;
    size_t exponentStartIdx = strSize;
    size_t idx = 0;

    // process number before decimal point
    double wholePart = 0.0;
    while (idx < strSize)
    {
        char ch = str[idx];

        if (ch == '.')
        {
            fractionStartIdx = idx + 1;

            // find exponent start
            while (idx < strSize)
            {
                ++idx;
                ch = str[idx];

                if (ch == 'e')
                {
                    exponentStartIdx = idx + 1;
                    break;
                }
            }
            break;
        }
        else if (ch == 'e')
        {
            exponentStartIdx = idx + 1;
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
    if (fractionStartIdx < strSize)
    {
        // reset index to last fraction digit
        if (exponentStartIdx < strSize)
        {
            idx = exponentStartIdx - 2;
        }
        else
        {
            idx = strSize - 1;
        }

        while (idx >= fractionStartIdx)
        {
            char ch = str[idx];

            if (ch != '_')
            {
                fractionalPart += ch - '0';
                fractionalPart /= 10.0;
            }

            --idx;
        }
    }

    double num = wholePart + fractionalPart;

    if (exponentStartIdx < strSize)
    {
        int neg = 1;
        if (str[exponentStartIdx] == '-')
        {
            neg = -1;
            idx = exponentStartIdx + 1;
        }
        else
        {
            idx = exponentStartIdx;
        }

        int exp = 0;
        while (idx < strSize)
        {
            char ch = str[idx];

            if (ch != '_')
            {
                exp *= 10;
                exp += ch - '0';
            }

            ++idx;
        }

        exp *= neg;

        num *= pow(10.0, exp);
    }

    return num;
}
