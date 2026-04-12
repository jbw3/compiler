#include "Token.h"

Token noneToken(ROString("", 0), 0, 0, 0, Token::Invalid);
const Token* Token::None = &noneToken;

Token::Token(ROString value, unsigned filenameId, unsigned line, unsigned column, uint16_t type) :
    value(value),
    filenameId(filenameId),
    line(line),
    column(column),
    type(type)
{
}
