#include "CompilerContext.h"
#include <cassert>
#include <cstring>
#include <stdlib.h>

using namespace std;

TokenValues::TokenValues()
{
    // TODO: use linked list instead of array
    buffArrayCapacity = 16;
    buffArray = new char*[buffArrayCapacity];
    buffArraySize = 0;

    buffCapacity = 1024;
    buff = new char[buffCapacity];
    buffEnd = buff + buffCapacity;
    buffArray[buffArraySize++] = buff;

    current = buff;
    end = buff;
}

TokenValues::~TokenValues()
{
    for (size_t i = 0; i < buffArraySize; ++i)
    {
        delete [] buffArray[i];
    }

    delete [] buffArray;
}

void TokenValues::AppendChar(char ch)
{
    if (end == buffEnd)
    {
        // TODO: expand if needed
        assert(buffArraySize < buffArrayCapacity);

        // allocate new buffer
        buffCapacity *= 2;
        buff = new char[buffCapacity];
        buffEnd = buff + buffCapacity;
        buffArray[buffArraySize++] = buff;

        // calculate size of current value
        size_t currentSize = end - current;

        // copy current data from last buffer
        memcpy(buff, current, currentSize);

        current = buff;
        end = buff + currentSize;
    }

    *end = ch;
    ++end;
}

CompilerContext::CompilerContext()
{
}

unsigned CompilerContext::AddFilename(const string& filename)
{
    filenames.push_back(filename);
    return static_cast<unsigned>(filenames.size() - 1);
}
