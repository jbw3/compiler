#include "Token.h"

using namespace std;

Token noneToken("", "", 0, 0, Token::eInvalid);
const Token* Token::None = &noneToken;

Token::Token(const string& value, const string& filename, unsigned line, unsigned column, EType type) :
    value(value),
    filename(filename),
    line(line),
    column(column),
    type(type)
{
}
