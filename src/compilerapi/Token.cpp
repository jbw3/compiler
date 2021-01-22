#include "Token.h"

using namespace std;

Token noneToken("", 0, 0, 0, Token::eInvalid);
const Token* Token::None = &noneToken;

Token::Token(const string& value, unsigned filenameId, unsigned line, unsigned column, EType type) :
    value(value),
    filenameId(filenameId),
    line(line),
    column(column),
    type(type)
{
}
