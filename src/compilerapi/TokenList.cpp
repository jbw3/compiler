#include "TokenList.h"
#include <string.h>

TokenList::TokenList()
{
    capacity = 256;
    size = 0;
    data = static_cast<Token*>(operator new[](capacity * sizeof(Token)));
}

TokenList::TokenList(std::initializer_list<Token> initList)
{
    size_t initListSize = initList.size();
    capacity = (initListSize == 0) ? 1 : initListSize;
    data = static_cast<Token*>(operator new[](capacity * sizeof(Token)));

    size = 0;
    for (const Token* t = initList.begin(); t != initList.end(); ++t)
    {
        data[size] = *t;
        ++size;
    }
}

TokenList::TokenList(const TokenList& other)
{
    capacity = other.capacity;
    size = other.size;

    data = static_cast<Token*>(operator new[](capacity * sizeof(Token)));
    memcpy(data, other.data, size * sizeof(Token));
}

TokenList::~TokenList()
{
    operator delete [](data);
}

void TokenList::Append(const Token& token)
{
    // resize if necessary
    if (size >= capacity)
    {
        if (capacity >= (1 << 20))
        {
            capacity *= 2;
        }
        else
        {
            capacity *= 4;
        }

        Token* oldData = data;
        data = static_cast<Token*>(operator new[](capacity * sizeof(Token)));
        memcpy(data, oldData, size * sizeof(Token));

        operator delete [](oldData);
    }

    data[size] = token;
    ++size;
}
