#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

class Token
{
public:
    static const Token* None;

    Token(const std::string& value, const std::string& filename, unsigned line, unsigned column);

    std::string value;
    std::string filename;
    unsigned line;
    unsigned column;
};

#endif // TOKEN_H_
