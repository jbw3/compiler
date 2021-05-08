#ifndef COMPILER_CONTEXT_H_
#define COMPILER_CONTEXT_H_

#include "TokenList.h"
#include <string>
#include <vector>

class TokenValues
{
public:
    TokenValues();

    ~TokenValues();

    void AppendChar(char ch);

    const char* EndValue()
    {
        AppendChar('\0');
        return current;
    }

    void StartNew()
    {
        current = end;
    }

    void ClearCurrent()
    {
        end = current;
    }

private:
    char* head;

    size_t buffCapacity;
    char* buffEnd;

    char* current;
    char* end;
};

union ConstantValue
{
    bool boolValue;
    int64_t intValue;
};

class CompilerContext
{
public:
    TokenValues tokenValues;

    CompilerContext();

    unsigned AddFile(const std::string& filename);

    unsigned GetFileIdCount() const
    {
        return static_cast<unsigned>(filenames.size());
    }

    const std::string& GetFilename(unsigned id) const
    {
        return filenames[id];
    }

    TokenList& GetFileTokens(unsigned id)
    {
        return fileTokens[id];
    }

    unsigned AddConstantValue(ConstantValue value);

    const ConstantValue& GetConstantValue(unsigned id) const
    {
        return constantValues[id];
    }

private:
    std::vector<std::string> filenames;
    std::vector<TokenList> fileTokens;
    std::vector<ConstantValue> constantValues;
};

#endif // COMPILER_CONTEXT_H_
