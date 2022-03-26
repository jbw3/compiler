#include "CompilerContext.h"
#include <cassert>
#include <cstring>
#include <stdlib.h>

using namespace std;

CompilerContext::CompilerContext(Config config, ostream& logStream) :
    config(config),
    logger(*this, &logStream, config.color),
    typeRegistry(config.targetMachine),
    basicTypeCount(0)
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

void CompilerContext::InitBasicTypes()
{
    AddTypeConstantValue(TypeInfo::BoolType);
    AddTypeConstantValue(TypeInfo::Int8Type);
    AddTypeConstantValue(TypeInfo::Int16Type);
    AddTypeConstantValue(TypeInfo::Int32Type);
    AddTypeConstantValue(TypeInfo::Int64Type);
    AddTypeConstantValue(typeRegistry.GetIntSizeType());
    AddTypeConstantValue(TypeInfo::UInt8Type);
    AddTypeConstantValue(TypeInfo::UInt16Type);
    AddTypeConstantValue(TypeInfo::UInt32Type);
    AddTypeConstantValue(TypeInfo::UInt64Type);
    AddTypeConstantValue(typeRegistry.GetUIntSizeType());
    AddTypeConstantValue(typeRegistry.GetStringType());
    AddTypeConstantValue(TypeInfo::Float32Type);
    AddTypeConstantValue(TypeInfo::Float64Type);
    AddTypeConstantValue(TypeInfo::TypeType);

    basicTypeCount = static_cast<unsigned>(typeConstants.size());
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

unsigned CompilerContext::AddTypeConstantValue(const TypeInfo* value)
{
    unsigned id = 0;
    const string& uniqueName = value->GetUniqueName();
    auto iter = typeConstantsIdMap.find(uniqueName);
    if (iter != typeConstantsIdMap.cend())
    {
        id = iter->second;
    }
    else
    {
        id = static_cast<unsigned>(typeConstants.size());
        typeConstants.push_back(value);
        typeConstantsIdMap.insert({uniqueName, id});
    }

    return id;
}
