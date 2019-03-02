#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

class Token
{
public:
    Token(const std::string& value, unsigned long line, unsigned long column);

    const std::string& GetValue() const;

    unsigned long GetLine() const;

    unsigned long GetColumn() const;

private:
    std::string value;
    unsigned long line;
    unsigned long column;
};

#endif // TOKEN_H_
