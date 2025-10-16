#include "CompilerContext.h"
#include <cassert>
#include <cstring>
#include <stdlib.h>

using namespace std;

StringBuilder::StringBuilder()
{
    buffSize = 1024;
    head = new char[buffSize];
    buffEnd = head + buffSize;

    // reserve the first few bytes as a pointer to the next buffer
    memset(head, 0, sizeof(char*));

    current = head + sizeof(char*);
    end = current;
}

StringBuilder::~StringBuilder()
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

void StringBuilder::AppendBuff(const char* ptr, size_t size)
{
    size_t freeSize = buffEnd - end;
    if (size > freeSize)
    {
        char* oldHead = head;

        // allocate new buffer
        buffSize *= 2;
        head = new char[buffSize];
        buffEnd = head + buffSize;

        // point this block of memory to the next block
        char** next = reinterpret_cast<char**>(head);
        *next = oldHead;

        // calculate size of current value
        size_t currentSize = end - current;

        // copy current data from last buffer
        char* newCurrent = head + sizeof(char*);
        memcpy(newCurrent, current, currentSize);

        current = newCurrent;
        end = current + currentSize;
    }

    memcpy(end, ptr, size);
    end += size;
}

CompilerContext::CompilerContext(Config config, ostream& logStream) :
    config(config),
    logger(*this, &logStream, config.color),
    typeRegistry(*this)
{
}

CompilerContext::~CompilerContext()
{
    for (size_t i = 0; i < fileBuffers.size(); ++i)
    {
        delete [] fileBuffers[i].ptr;
    }
}

unsigned CompilerContext::AddFile(const string& filename, CharBuffer fileBuffer)
{
    unsigned id = static_cast<unsigned>(filenames.size());
    filenames.push_back(filename);
    fileBuffers.push_back(fileBuffer);
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

unsigned CompilerContext::AddFloatConstantValue(double value)
{
    unsigned id = static_cast<unsigned>(floatConstants.size());
    floatConstants.push_back(value);

    return id;
}

unsigned CompilerContext::AddStrConstantValue(vector<char> value)
{
    unsigned id = static_cast<unsigned>(strConstants.size());
    strConstants.push_back(value);

    return id;
}

unsigned CompilerContext::AddRangeConstantValue(const RangeConstValue& value)
{
    unsigned id = static_cast<unsigned>(rangeConstants.size());
    rangeConstants.push_back(value);

    return id;
}

unsigned CompilerContext::AddStructConstantValue(const StructConstValue& value)
{
    unsigned id = static_cast<unsigned>(structConstants.size());
    structConstants.push_back(value);

    return id;
}

unsigned CompilerContext::AddArrayConstantValue(const ArrayConstValue& value)
{
    unsigned id = static_cast<unsigned>(arrayConstants.size());
    arrayConstants.push_back(value);

    return id;
}

size_t CompilerContext::GetArrayConstantValueSize(unsigned arrayId) const
{
    const ArrayConstValue& arrayValue = GetArrayConstantValue(arrayId);
    size_t arraySize = GetArrayConstantValueSize(arrayValue);
    return arraySize;
}

size_t CompilerContext::GetArrayConstantValueSize(const ArrayConstValue& value) const
{
    size_t arraySize = 0;
    if (value.type == ArrayConstValue::eMultiValue)
    {
        arraySize = value.valueIndices.size();
    }
    else
    {
        unsigned sizeIdx = value.valueIndices[0];
        int64_t sizeValue = GetIntConstantValue(sizeIdx);
        arraySize = static_cast<size_t>(sizeValue);
    }

    return arraySize;
}

unsigned CompilerContext::AddFunctionConstantValue(const SyntaxTree::FunctionDeclaration* value)
{
    unsigned id = static_cast<unsigned>(functionConstants.size());
    functionConstants.push_back(value);

    return id;
}
