#ifndef UTILS_H_
#define UTILS_H_

#include <cctype>
#include <cstring>
#include "ROString.h"

constexpr bool is1ByteUtf8(char ch)
{
    return (ch & 0x80) == 0;
}

constexpr bool is2ByteUtf8Start(char ch)
{
    return (ch & 0xe0) == 0xc0;
}

constexpr bool is3ByteUtf8Start(char ch)
{
    return (ch & 0xf0) == 0xe0;
}

constexpr bool is4ByteUtf8Start(char ch)
{
    return (ch & 0xf1) == 0xf0;
}

constexpr bool isUtf8Continuation(char ch)
{
    return (ch & 0xc0) == 0x80;
}

constexpr bool hexDigitToNum(char ch, char& num)
{
    if (ch >= '0' && ch <= '9')
    {
        num = ch - '0';
        return true;
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        num = ch - 'A' + 10;
        return true;
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        num = ch - 'a' + 10;
        return true;
    }
    else
    {
        num = '\0';
        return false;
    }
}

bool stringToInteger(ROString str, int64_t& num);

double stringToFloat(ROString str);

constexpr int64_t getBitMask(unsigned size)
{
    int64_t mask = 0;
    switch (size)
    {
        case 8:
            mask = 0xff;
            break;
        case 16:
            mask = 0xffff;
            break;
        case 32:
            mask = 0xffff'ffff;
            break;
        case 64:
            mask = 0xffff'ffff'ffff'ffff;
            break;
    }

    return mask;
}

template<typename T>
void deletePointerContainer(T& container)
{
    for (auto ptr : container)
    {
        delete ptr;
    }
    container.clear();
}

#endif // UTILS_H_
