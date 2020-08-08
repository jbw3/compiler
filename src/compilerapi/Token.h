#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

class Token
{
public:
    static const Token* None;

    Token(const std::string& value, const std::string& filename, unsigned long line, unsigned long column);

    const std::string& GetValue() const;

    const std::string& GetFilename() const;

    unsigned long GetLine() const;

    unsigned long GetColumn() const;

private:
    std::string value;
    std::string filename;
    unsigned long line;
    unsigned long column;
};

#endif // TOKEN_H_
