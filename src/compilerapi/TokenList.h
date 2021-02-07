#ifndef TOKEN_LIST_H_
#define TOKEN_LIST_H_

#include "Token.h"
#include <initializer_list>
#include <stddef.h>

class TokenList
{
public:
    typedef const Token* const_iterator;

    TokenList();

    TokenList(std::initializer_list<Token> initList);

    TokenList(const TokenList& other);

    ~TokenList();

    void Append(const Token& token);

    void Clear()
    {
        size = 0;
    }

    size_t GetSize() const
    {
        return size;
    }

    const Token& operator[](size_t index) const
    {
        return data[index];
    }

    const_iterator begin() const
    {
        return data;
    }

    const_iterator end() const
    {
        return data + size;
    }

private:
    size_t capacity;
    size_t size;
    Token* data;
};

#endif // TOKEN_LIST_H_
