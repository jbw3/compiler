#ifndef UTILS_H_
#define UTILS_H_

#include <cctype>
#include <string>

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

bool isIdentifier(const std::string& str);

bool isNumber(const std::string& tokenStr);

bool isBool(const std::string& tokenStr);

bool stringToInteger(const std::string& str, int64_t& num);

bool stringToBool(const std::string& str, bool& value);

#endif // UTILS_H_
