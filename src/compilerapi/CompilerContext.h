#ifndef COMPILER_CONTEXT_H_
#define COMPILER_CONTEXT_H_

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

    unsigned AddFilename(const std::string& filename);

    const std::string& GetFilename(unsigned id) const
    {
        return filenames[id];
    }

private:
    std::vector<std::string> filenames;
};

#endif // COMPILER_CONTEXT_H_
