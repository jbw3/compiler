#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "SyntaxTree.h"
#include "TypeInfo.h"
#include "keywords.h"
#include "TypeRegistry.h"
#include "llvm/Target/TargetMachine.h"
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

TypeRegistry::TypeRegistry(const TargetMachine* targetMachine)
{
    pointerSize = 8 * targetMachine->getAllocaPointerSize();

    types.insert({BOOL_KEYWORD, TypeInfo::BoolType});
    types.insert({INT8_KEYWORD, TypeInfo::Int8Type});
    types.insert({INT16_KEYWORD, TypeInfo::Int16Type});
    types.insert({INT32_KEYWORD, TypeInfo::Int32Type});
    types.insert({INT64_KEYWORD, TypeInfo::Int64Type});
    types.insert({UINT8_KEYWORD, TypeInfo::UInt8Type});
    types.insert({UINT16_KEYWORD, TypeInfo::UInt16Type});
    types.insert({UINT32_KEYWORD, TypeInfo::UInt32Type});
    types.insert({UINT64_KEYWORD, TypeInfo::UInt64Type});
    types.insert({FLOAT32_KEYWORD, TypeInfo::Float32Type});
    types.insert({FLOAT64_KEYWORD, TypeInfo::Float64Type});
    types.insert({TYPE_KEYWORD, TypeInfo::TypeType});

    intSizeType = new PrimitiveType(pointerSize, TypeInfo::F_INT, TypeInfo::eSigned, INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    RegisterType(intSizeType);

    uintSizeType = new PrimitiveType(pointerSize, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);

    stringType = new StringType(pointerSize * 2, uintSizeType, GetPointerToType(TypeInfo::UInt8Type));
    RegisterType(stringType);
}

unsigned TypeRegistry::GetPointerSize() const
{
    return pointerSize;
}

const TypeInfo* TypeRegistry::GetIntSizeType() const
{
    return intSizeType;
}

const TypeInfo* TypeRegistry::GetUIntSizeType() const
{
    return uintSizeType;
}

const TypeInfo* TypeRegistry::GetStringType() const
{
    return stringType;
}

const TypeInfo* TypeRegistry::GetRangeType(const TypeInfo* memberType, bool isHalfOpen)
{
    string uniqueName = "Range";
    uniqueName += (isHalfOpen ? "HalfOpen" : "Closed");
    uniqueName += "'" + memberType->GetUniqueName() + "'";
    const TypeInfo* rangeType = GetType(uniqueName);
    if (rangeType == nullptr)
    {
        unsigned size = memberType->GetNumBits() * 2;
        uint16_t flags = TypeInfo::F_RANGE | TypeInfo::F_AGGREGATE | (isHalfOpen ? TypeInfo::F_HALF_OPEN : TypeInfo::F_NONE);

        string name = "Range";
        name += (isHalfOpen ? "HalfOpen" : "Closed");
        name += "'" + memberType->GetShortName() + "'";

        TypeInfo* newRangeType = new PrimitiveType(size, flags, TypeInfo::eNotApplicable, uniqueName, name, memberType);
        newRangeType->AddMember("Start", memberType, false, Token::None);
        newRangeType->AddMember("End", memberType, false, Token::None);

        rangeType = newRangeType;
    }

    return rangeType;
}

const TypeInfo* TypeRegistry::GetFunctionType(const FunctionDeclaration* functionDeclaration)
{
    const Parameters& parameters = functionDeclaration->parameters;
    string uniqueName = "fun(";
    string name = "fun(";
    if (parameters.size() > 0)
    {
        uniqueName += parameters[0]->type->GetUniqueName();
        name += parameters[0]->type->GetShortName();

        for (size_t i = 1; i < parameters.size(); ++i)
        {
            const TypeInfo* paramType = parameters[i]->type;

            uniqueName += ", ";
            uniqueName += paramType->GetUniqueName();

            name += ", ";
            name += paramType->GetShortName();
        }
    }

    uniqueName += ")";
    name += ")";

    uniqueName += functionDeclaration->returnType->GetUniqueName();
    if (!functionDeclaration->returnType->IsUnit())
    {
        name += ' ';
        name += functionDeclaration->returnType->GetShortName();
    }

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        // add param and return types
        vector<const TypeInfo*> parameterTypes;
        vector<string> parameterNames;
        for (const Parameter* param : parameters)
        {
            parameterTypes.push_back(param->type);
            parameterNames.push_back(param->name);
        }

        funType = TypeInfo::CreateFunctionType(GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, functionDeclaration->returnType);
    }

    return funType;
}

const TypeInfo* TypeRegistry::GetFunctionType(
        const vector<const TypeInfo*>& parameterTypes,
        const vector<string>& parameterNames,
        const TypeInfo* returnType
)
{
    size_t paramSize = parameterTypes.size();
    string uniqueName = "fun(";
    string name = "fun(";
    if (paramSize > 0)
    {
        uniqueName += parameterTypes[0]->GetUniqueName();
        name += parameterTypes[0]->GetShortName();

        for (size_t i = 1; i < paramSize; ++i)
        {
            const TypeInfo* paramType = parameterTypes[i];

            uniqueName += ", ";
            uniqueName += paramType->GetUniqueName();

            name += ", ";
            name += paramType->GetShortName();
        }
    }

    uniqueName += ")";
    name += ")";

    uniqueName += returnType->GetUniqueName();
    if (!returnType->IsUnit())
    {
        name += ' ';
        name += returnType->GetShortName();
    }

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        funType = TypeInfo::CreateFunctionType(GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, returnType);
    }

    return funType;
}

const TypeInfo* TypeRegistry::GetPointerToType(const TypeInfo* type)
{
    string uniqueName = POINTER_TYPE_TOKEN + type->GetUniqueName();
    const TypeInfo* ptrType = GetType(uniqueName);
    if (ptrType == nullptr)
    {
        string name = POINTER_TYPE_TOKEN + type->GetShortName();
        TypeInfo* newPtrType = new PrimitiveType(pointerSize, TypeInfo::F_POINTER, TypeInfo::eNotApplicable, uniqueName, name, type);
        RegisterType(newPtrType);

        ptrType = newPtrType;
    }

    return ptrType;
}

const TypeInfo* TypeRegistry::GetArrayOfType(const TypeInfo* type)
{
    string uniqueName;
    uniqueName += ARRAY_TYPE_START_TOKEN;
    uniqueName += ARRAY_TYPE_END_TOKEN;
    uniqueName += type->GetUniqueName();
    const TypeInfo* arrayType = GetType(uniqueName);
    if (arrayType == nullptr)
    {
        string name;
        name += ARRAY_TYPE_START_TOKEN;
        name += ARRAY_TYPE_END_TOKEN;
        name += type->GetShortName();

        TypeInfo* newArrayType = new PrimitiveType(pointerSize * 2, TypeInfo::F_ARRAY, TypeInfo::eNotApplicable, uniqueName, name, type);
        newArrayType->AddMember("Size", GetUIntSizeType(), false, Token::None);
        newArrayType->AddMember("Data", GetPointerToType(type), false, Token::None);
        RegisterType(newArrayType);

        arrayType = newArrayType;
    }

    return arrayType;
}

bool TypeRegistry::RegisterType(const TypeInfo* typeInfo)
{
    auto pair = types.insert({ typeInfo->GetUniqueName(), typeInfo });
    return pair.second;
}

const TypeInfo* TypeRegistry::GetType(const string& typeName)
{
    auto iter = types.find(typeName);
    if (iter == types.cend())
    {
        return nullptr;
    }
    return iter->second;
}
