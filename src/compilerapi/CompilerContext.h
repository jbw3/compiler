#ifndef COMPILER_CONTEXT_H_
#define COMPILER_CONTEXT_H_

#include "TokenList.h"
#include "TypeInfo.h"
#include <string>
#include <vector>

namespace SyntaxTree
{
class FunctionDeclaration;
}

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

struct RangeConstValue
{
    int64_t start;
    int64_t end;
};

struct StructConstValue
{
    std::vector<unsigned> memberIndices;
};

struct ArrayConstValue
{
    std::vector<unsigned> valueIndices;
    enum EType
    {
        eSizeValue,
        eMultiValue,
    } type;
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

    unsigned AddStrConstantValue(std::vector<char> value);

    std::vector<char> GetStrConstantValue(unsigned id) const
    {
        return strConstants[id];
    }

    unsigned AddRangeConstantValue(const RangeConstValue& value);

    const RangeConstValue& GetRangeConstantValue(unsigned id) const
    {
        return rangeConstants[id];
    }

    unsigned AddStructConstantValue(const StructConstValue& value);

    const StructConstValue& GetStructConstantValue(unsigned id) const
    {
        return structConstants[id];
    }

    unsigned AddArrayConstantValue(const ArrayConstValue& value);

    const ArrayConstValue& GetArrayConstantValue(unsigned id) const
    {
        return arrayConstants[id];
    }

    size_t GetArrayConstantValueSize(unsigned arrayId) const;

    size_t GetArrayConstantValueSize(const ArrayConstValue& value) const;

    unsigned AddFunctionConstantValue(const SyntaxTree::FunctionDeclaration* value);

    const SyntaxTree::FunctionDeclaration* GetFunctionConstantValue(unsigned id) const
    {
        return functionConstants[id];
    }

    unsigned AddTypeConstantValue(const TypeInfo* value);

    const TypeInfo* GetTypeConstantValue(unsigned id) const
    {
        return typeConstants[id];
    }

private:
    std::vector<std::string> filenames;
    std::vector<TokenList> fileTokens;
    std::vector<int64_t> intConstants;
    std::vector<std::vector<char>> strConstants;
    std::vector<RangeConstValue> rangeConstants;
    std::vector<StructConstValue> structConstants;
    std::vector<ArrayConstValue> arrayConstants;
    std::vector<const SyntaxTree::FunctionDeclaration*> functionConstants;
    std::vector<const TypeInfo*> typeConstants;
};

#endif // COMPILER_CONTEXT_H_
