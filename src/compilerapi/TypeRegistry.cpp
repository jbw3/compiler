#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "CompilerContext.h"
#include "SyntaxTree.h"
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

FunctionTypeKey::FunctionTypeKey(const FunctionDeclaration* functionDeclaration)
{
    paramTypeIds.reserve(functionDeclaration->parameters.size());
    for (const Parameter* param : functionDeclaration->parameters)
    {
        paramTypeIds.push_back(param->type->GetId());
    }

    returnTypeId = functionDeclaration->returnType->GetId();
}

FunctionTypeKey::FunctionTypeKey(const vector<const TypeInfo*>& parameterTypes, const TypeInfo* returnType)
{
    paramTypeIds.reserve(parameterTypes.size());
    for (const TypeInfo* paramType : parameterTypes)
    {
        paramTypeIds.push_back(paramType->GetId());
    }

    returnTypeId = returnType->GetId();
}

bool FunctionTypeKey::operator ==(const FunctionTypeKey& other) const
{
    if (returnTypeId != other.returnTypeId)
    {
        return false;
    }

    size_t size = paramTypeIds.size();
    if (size != other.paramTypeIds.size())
    {
        return false;
    }

    for (size_t i = 0; i < size; ++i)
    {
        if (paramTypeIds[i] != other.paramTypeIds[i])
        {
            return false;
        }
    }

    return true;
}

TypeRegistry::TypeRegistry(CompilerContext& compilerContext) :
    compilerContext(compilerContext)
{
    types.push_back(TypeInfo::UnitType);
    types.push_back(TypeInfo::BoolType);
    types.push_back(TypeInfo::Int8Type);
    types.push_back(TypeInfo::Int16Type);
    types.push_back(TypeInfo::Int32Type);
    types.push_back(TypeInfo::Int64Type);
    types.push_back(TypeInfo::UInt8Type);
    types.push_back(TypeInfo::UInt16Type);
    types.push_back(TypeInfo::UInt32Type);
    types.push_back(TypeInfo::UInt64Type);
    types.push_back(TypeInfo::Float32Type);
    types.push_back(TypeInfo::Float64Type);
    types.push_back(TypeInfo::Float32LiteralType);
    types.push_back(TypeInfo::Float64LiteralType);
    types.push_back(TypeInfo::TypeType);
    typeDeleteStartIndex = types.size();

    assert(GetNextTypeId() == 15 && "Unexpected next type ID");

    pointerSize = 8 * compilerContext.config.targetMachine->getAllocaPointerSize();

    intSizeType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_INT, TypeInfo::eSigned, INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    types.push_back(intSizeType);

    uintSizeType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    types.push_back(uintSizeType);

    stringType = new TypeInfo(GetNextTypeId(), pointerSize * 2, TypeInfo::F_STR, TypeInfo::eNotApplicable, STR_KEYWORD, STR_KEYWORD);
    types.push_back(stringType);
    stringType->AddMember("Size", uintSizeType, false, Token::None);
    stringType->AddMember("Data", GetPointerToType(TypeInfo::UInt8Type), false, Token::None);
}

TypeRegistry::~TypeRegistry()
{
    size_t size = types.size();
    for (size_t i = typeDeleteStartIndex; i < size; ++i)
    {
        delete types[i];
    }
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
    TypeId memberTypeId = memberType->GetId();
    tuple<TypeId, bool> key(memberTypeId, isHalfOpen);

    auto iter = rangeTypes.find(key);
    if (iter != rangeTypes.end())
    {
        return iter->second;
    }

    ROString uniqueName = compilerContext.stringBuilder
        .Append("Range")
        .Append(isHalfOpen ? "HalfOpen" : "Closed")
        .Append("'")
        .Append(memberType->GetUniqueName())
        .Append("'")
        .CreateString();

    unsigned size = memberType->GetNumBits() * 2;
    uint16_t flags = TypeInfo::F_RANGE | (isHalfOpen ? TypeInfo::F_HALF_OPEN : TypeInfo::F_NONE);

    ROString name = compilerContext.stringBuilder
        .Append("Range")
        .Append(isHalfOpen ? "HalfOpen" : "Closed")
        .Append("'")
        .Append(memberType->GetShortName())
        .Append("'")
        .CreateString();

    TypeInfo* newRangeType = new TypeInfo(GetNextTypeId(), size, flags, TypeInfo::eNotApplicable, uniqueName, name, memberType);
    types.push_back(newRangeType);
    newRangeType->AddMember("Start", memberType, false, Token::None);
    newRangeType->AddMember("End", memberType, false, Token::None);
    rangeTypes.insert({key, newRangeType});

    return newRangeType;
}

static ROString getFunctionUniqueName(
    StringBuilder& sb,
    const vector<const TypeInfo*>& parameterTypes,
    const TypeInfo* returnType)
{
    size_t paramSize = parameterTypes.size();
    sb.Append("fun(");
    if (paramSize > 0)
    {
        sb.Append(parameterTypes[0]->GetUniqueName());

        for (size_t i = 1; i < paramSize; ++i)
        {
            const TypeInfo* paramType = parameterTypes[i];

            sb.Append(", ");
            sb.Append(paramType->GetUniqueName());
        }
    }

    sb.Append(")");
    sb.Append(returnType->GetUniqueName());

    return sb.CreateString();
}

static ROString getFunctionName(
    StringBuilder& sb,
    const vector<const TypeInfo*>& parameterTypes,
    const TypeInfo* returnType)
{
    size_t paramSize = parameterTypes.size();
    sb.Append("fun(");
    if (paramSize > 0)
    {
        sb.Append(parameterTypes[0]->GetShortName());

        for (size_t i = 1; i < paramSize; ++i)
        {
            const TypeInfo* paramType = parameterTypes[i];

            sb.Append(", ");
            sb.Append(paramType->GetShortName());
        }
    }

    sb.Append(")");

    if (!returnType->IsUnit())
    {
        sb.Append(' ');
        sb.Append(returnType->GetShortName());
    }

    return sb.CreateString();
}

TypeInfo* TypeRegistry::GetAggregateType(ROString name, const Token* token)
{
    TypeInfo* newType = new TypeInfo(GetNextTypeId(), 0, TypeInfo::F_AGGREGATE, TypeInfo::eNotApplicable, name, name, nullptr, token);
    types.push_back(newType);
    return newType;
}

const TypeInfo* TypeRegistry::GetFunctionType(const FunctionDeclaration* functionDeclaration)
{
    FunctionTypeKey key(functionDeclaration);

    auto iter = functionTypes.find(key);
    if (iter != functionTypes.end())
    {
        return iter->second;
    }

    const Parameters& parameters = functionDeclaration->parameters;
    vector<const TypeInfo*> parameterTypes;
    vector<ROString> parameterNames;
    for (const Parameter* param : parameters)
    {
        parameterTypes.push_back(param->type);
        parameterNames.push_back(param->name);
    }
    const TypeInfo* returnType = functionDeclaration->returnType;

    ROString uniqueName = getFunctionUniqueName(compilerContext.stringBuilder, parameterTypes, returnType);
    ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, returnType);
    // add param and return types
    const TypeInfo* newFunType = TypeInfo::CreateFunctionType(GetNextTypeId(), GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, returnType);
    types.push_back(newFunType);
    functionTypes.insert({key, newFunType});

    return newFunType;
}

const TypeInfo* TypeRegistry::GetFunctionType(
        const vector<const TypeInfo*>& parameterTypes,
        const vector<ROString>& parameterNames,
        const TypeInfo* returnType
)
{
    FunctionTypeKey key(parameterTypes, returnType);

    auto iter = functionTypes.find(key);
    if (iter != functionTypes.end())
    {
        return iter->second;
    }

    ROString uniqueName = getFunctionUniqueName(compilerContext.stringBuilder, parameterTypes, returnType);
    ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, returnType);
    const TypeInfo* newFunType = TypeInfo::CreateFunctionType(GetNextTypeId(), GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, returnType);
    types.push_back(newFunType);
    functionTypes.insert({key, newFunType});

    return newFunType;
}

const TypeInfo* TypeRegistry::GetPointerToType(const TypeInfo* type)
{
    TypeId typeId = type->GetId();

    auto iter = pointerTypes.find(typeId);
    if (iter != pointerTypes.end())
    {
        return iter->second;
    }

    ROString uniqueName = compilerContext.stringBuilder
        .Append(POINTER_TYPE_TOKEN)
        .Append(type->GetUniqueName())
        .CreateString();
    ROString name = compilerContext.stringBuilder
        .Append(POINTER_TYPE_TOKEN)
        .Append(type->GetShortName())
        .CreateString();
    TypeInfo* newPtrType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_POINTER, TypeInfo::eNotApplicable, uniqueName, name, type);
    types.push_back(newPtrType);
    pointerTypes.insert({typeId, newPtrType});

    return newPtrType;
}

const TypeInfo* TypeRegistry::GetArrayOfType(const TypeInfo* type)
{
    TypeId typeId = type->GetId();

    auto iter = arrayTypes.find(typeId);
    if (iter != arrayTypes.end())
    {
        return iter->second;
    }

    ROString uniqueName = compilerContext.stringBuilder
        .Append(ARRAY_TYPE_START_TOKEN)
        .Append(ARRAY_TYPE_END_TOKEN)
        .Append(type->GetUniqueName())
        .CreateString();
    ROString name = compilerContext.stringBuilder
        .Append(ARRAY_TYPE_START_TOKEN, ARRAY_TYPE_END_TOKEN, type->GetShortName())
        .CreateString();

    TypeInfo* newArrayType = new TypeInfo(GetNextTypeId(), pointerSize * 2, TypeInfo::F_ARRAY, TypeInfo::eNotApplicable, uniqueName, name, type);
    types.push_back(newArrayType);
    newArrayType->AddMember("Size", GetUIntSizeType(), false, Token::None);
    newArrayType->AddMember("Data", GetPointerToType(type), false, Token::None);
    arrayTypes.insert({typeId, newArrayType});

    return newArrayType;
}

const TypeInfo* TypeRegistry::GetTypeAlias(ROString newName, const Token* newToken, const TypeInfo *typeInfo)
{
    const TypeInfo* newTypeInfo = TypeInfo::CreateTypeAlias(newName, newToken, typeInfo);
    return newTypeInfo;
}

const NumericLiteralType* TypeRegistry::CreateNumericLiteralType(unsigned signedNumBits, unsigned unsignedNumBits)
{
    return CreateNumericLiteralType(TypeInfo::eContextDependent, signedNumBits, unsignedNumBits, "{integer}");
}

const NumericLiteralType* TypeRegistry::CreateSignedNumericLiteralType(unsigned numBits)
{
    return CreateNumericLiteralType(TypeInfo::eSigned, numBits, 0, "{signed-integer}");
}

const NumericLiteralType* TypeRegistry::CreateUnsignedNumericLiteralType(unsigned numBits)
{
    return CreateNumericLiteralType(TypeInfo::eUnsigned, 0, numBits, "{unsigned-integer}");
}

const TypeInfo* TypeRegistry::GetMinSizeNumericLiteralType(const NumericLiteralType* numLitType, TypeInfo::ESign sign)
{
    const TypeInfo* type = nullptr;
    switch (sign)
    {
        case TypeInfo::eNotApplicable:
            type = nullptr;
            break;
        case TypeInfo::eSigned:
            type = CreateSignedNumericLiteralType(numLitType->GetSignedNumBits());
            break;
        case TypeInfo::eUnsigned:
            type = CreateUnsignedNumericLiteralType(numLitType->GetUnsignedNumBits());
            break;
        case TypeInfo::eContextDependent:
            type = numLitType;
            break;
    }

    return type;
}

TypeId TypeRegistry::GetNextTypeId()
{
    return static_cast<TypeId>(types.size());
}

ROString TypeRegistry::GetNumericLiteralTypeUniqueName(unsigned signedNumBits, unsigned unsignedNumBits)
{
    StringBuilder& sb = compilerContext.stringBuilder;
    sb.Append("{integer-");
    sb.Append(to_string(signedNumBits));
    sb.Append("-");
    sb.Append(to_string(unsignedNumBits));
    sb.Append("}");

    return sb.CreateString();
}

const NumericLiteralType* TypeRegistry::CreateNumericLiteralType(TypeInfo::ESign sign, unsigned signedNumBits, unsigned unsignedNumBits, ROString name)
{
    const NumericLiteralType* type = nullptr;
    auto key = make_tuple(sign, signedNumBits, unsignedNumBits);

    auto iter = numericLiteralTypes.find(key);
    if (iter == numericLiteralTypes.end())
    {
        type = new NumericLiteralType(
            GetNextTypeId(),
            sign,
            signedNumBits,
            unsignedNumBits,
            GetNumericLiteralTypeUniqueName(signedNumBits, unsignedNumBits),
            name
        );
        types.push_back(type);
        numericLiteralTypes.insert({key, type});
    }
    else
    {
        type = iter->second;
    }

    return type;
}
