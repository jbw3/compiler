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

    unsigned AddBoolConstantValue(bool value)
    {
        // encode the value in the index
        return static_cast<unsigned>(value);
    }

    bool GetBoolConstantValue(unsigned id) const
    {
        // the value is encoded in the index
        return static_cast<bool>(id);
    }

    unsigned AddIntConstantValue(int64_t value);

    int64_t GetIntConstantValue(unsigned id) const;

    unsigned AddStrConstantValue(std::vector<char>* value);

    std::vector<char>* GetStrConstantValue(unsigned id) const
    {
        return strConstants[id];
    }

private:
    std::vector<std::string> filenames;
    std::vector<TokenList> fileTokens;
    std::vector<int64_t> intConstants;
    std::vector<std::vector<char>*> strConstants;
};

#endif // COMPILER_CONTEXT_H_
