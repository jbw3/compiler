#ifndef COMPILER_CONTEXT_H_
#define COMPILER_CONTEXT_H_

#include "Config.h"
#include "ErrorLogger.h"
#include "TokenList.h"
#include "TypeInfo.h"
#include "TypeRegistry.h"
#include <string>
#include <vector>

namespace SyntaxTree
{
class FunctionDeclaration;
}

struct CharBuffer
{
    size_t size;
    const char* ptr;
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

class StringBuilder
{
public:
    static constexpr size_t STRING_MEM_BLOCK_SIZE = 1024;

    StringBuilder();

    ~StringBuilder();

    template<typename... Ts>
    ROString CreateString(Ts... strings)
    {
        Append(strings...);
        return ROString(current, end - current);
    }

private:
    char* head;
    char* buffEnd;
    char* current;
    char* end;

    template<typename T, typename... Ts>
    void Append(T s, Ts... strings)
    {
        Append(s);
        Append(strings...);
    }

    void Append(const char* ptr, size_t size);

    void Append(const char* cStr)
    {
        Append(cStr, strlen(cStr));
    }

    void Append(ROString str)
    {
        Append(str.GetPtr(), str.GetSize());
    }
};

class CompilerContext
{
public:
    Config config;
    ErrorLogger logger;
    TypeRegistry typeRegistry;
    StringBuilder stringBuilder;

    CompilerContext(Config config, std::ostream& logStream);

    ~CompilerContext();

    unsigned AddFile(const std::string& filename, CharBuffer fileBuffer);

    unsigned GetFileIdCount() const
    {
        return static_cast<unsigned>(filenames.size());
    }

    const std::string& GetFilename(unsigned id) const
    {
        return filenames[id];
    }

    CharBuffer GetFileBuffer(unsigned id) const
    {
        return fileBuffers[id];
    }

    TokenList& GetFileTokens(unsigned id)
    {
        return fileTokens[id];
    }

    void InitBasicTypes();

    unsigned GetBasicTypeCount() const
    {
        return basicTypeCount;
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

    unsigned AddFloatConstantValue(double value);

    double GetFloatConstantValue(unsigned id) const
    {
        return floatConstants[id];
    }

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
    std::vector<CharBuffer> fileBuffers;
    std::vector<TokenList> fileTokens;
    unsigned basicTypeCount;
    std::vector<int64_t> intConstants;
    std::vector<double> floatConstants;
    std::vector<std::vector<char>> strConstants;
    std::vector<RangeConstValue> rangeConstants;
    std::vector<StructConstValue> structConstants;
    std::vector<ArrayConstValue> arrayConstants;
    std::vector<const SyntaxTree::FunctionDeclaration*> functionConstants;

    std::unordered_map<std::string, unsigned> typeConstantsIdMap;
    std::vector<const TypeInfo*> typeConstants;
};

#endif // COMPILER_CONTEXT_H_
