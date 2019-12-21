#ifndef UTILS_H_
#define UTILS_H_

#include <cctype>
#include <string>

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

constexpr bool isIdentifierChar(char ch, bool isFirstChar)
{
    if (isFirstChar)
    {
        return std::isalpha(ch) || ch == '_';
    }
    else
    {
        return std::isalnum(ch) || ch == '_';
    }
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

bool isIdentifier(const std::string& str);

bool isNumber(const std::string& tokenStr);

bool isPotentialNumber(const std::string& tokenStr);

bool isBool(const std::string& tokenStr);

bool isString(const std::string& tokenStr);

bool isPotentialString(const std::string& tokenStr);

bool stringToInteger(const std::string& str, int64_t& num);

bool stringToBool(const std::string& str, bool& value);

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
