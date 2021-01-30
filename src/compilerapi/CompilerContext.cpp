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

unsigned CompilerContext::AddFilename(const string& filename)
{
    filenames.push_back(filename);
    return static_cast<unsigned>(filenames.size() - 1);
}
