#include "Token.h"

Token noneToken("", 0, 0, 0, Token::eInvalid);
const Token* Token::None = &noneToken;

Token::Token(const char* value, unsigned filenameId, unsigned line, unsigned column, EType type) :
    value(value),
    filenameId(filenameId),
    line(line),
    column(column),
    type(type)
{
}
