#include "Token.h"

using namespace std;

Token noneToken("", 0, 0);
const Token* Token::None = &noneToken;

Token::Token(const string& value, unsigned long line, unsigned long column) :
    value(value),
    line(line),
    column(column)
{
}

const string& Token::GetValue() const
{
    return value;
}

unsigned long Token::GetLine() const
{
    return line;
}

unsigned long Token::GetColumn() const
{
    return column;
}
