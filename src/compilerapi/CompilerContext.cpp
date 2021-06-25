#include "CompilerContext.h"
#include <cassert>
#include <cstring>
#include <stdlib.h>

using namespace std;

TokenValues::TokenValues()
{
    buffCapacity = 1024;
    head = new char[buffCapacity];
    buffEnd = head + buffCapacity;

    // reserve the first few bytes as a pointer to the next buffer
    memset(head, 0, sizeof(void*));

    current = head + sizeof(void*);
    end = current;
}

TokenValues::~TokenValues()
{
    while (head != nullptr)
    {
        char* temp = head;

        // the first bytes in each block of memory point to the next block
        char** next = reinterpret_cast<char**>(head);
        head = *next;

        delete [] temp;
    }
}

void TokenValues::AppendChar(char ch)
{
    if (end == buffEnd)
    {
        char* oldHead = head;

        // allocate new buffer
        buffCapacity *= 2;
        head = new char[buffCapacity];
        buffEnd = head + buffCapacity;

        // point this block of memory to the next block
        char** next = reinterpret_cast<char**>(head);
        *next = oldHead;

        // calculate size of current value
        size_t currentSize = end - current;

        // copy current data from last buffer
        char* newCurrent = head + sizeof(void*);
        memcpy(newCurrent, current, currentSize);

        current = newCurrent;
        end = current + currentSize;
    }

    *end = ch;
    ++end;
}

CompilerContext::CompilerContext()
{
}

unsigned CompilerContext::AddFile(const string& filename)
{
    unsigned id = static_cast<unsigned>(filenames.size());
    filenames.push_back(filename);
    fileTokens.push_back(TokenList());

    return id;
}

constexpr int64_t INT_ENCODE_THRESHOLD = 256;

unsigned CompilerContext::AddIntConstantValue(int64_t value)
{
    unsigned id = 0;
    if (value >= 0 && value < INT_ENCODE_THRESHOLD)
    {
        // encode small values in the index
        id = static_cast<unsigned>(value);
    }
    else
    {
        id = static_cast<unsigned>(intConstants.size()) + INT_ENCODE_THRESHOLD;
        intConstants.push_back(value);
    }

    return id;
}

int64_t CompilerContext::GetIntConstantValue(unsigned id) const
{
    if (id < INT_ENCODE_THRESHOLD)
    {
        return static_cast<int64_t>(id);
    }
    else
    {
        return intConstants[id - INT_ENCODE_THRESHOLD];
    }
}

unsigned CompilerContext::AddStrConstantValue(vector<char>* value)
{
    unsigned id = static_cast<unsigned>(strConstants.size());
    strConstants.push_back(value);

    return id;
}
