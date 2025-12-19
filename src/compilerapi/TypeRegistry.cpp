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

    intSizeType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_INT, TypeInfo::eSigned, INT_SIZE_KEYWORD);
    types.push_back(intSizeType);

    uintSizeType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD);
    types.push_back(uintSizeType);

    stringType = new TypeInfo(GetNextTypeId(), pointerSize * 2, TypeInfo::F_STR, TypeInfo::eNotApplicable, STR_KEYWORD);
    types.push_back(stringType);
    stringType->AddMember("Size", uintSizeType, false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
    stringType->AddMember("Data", GetPointerToType(TypeInfo::UInt8Type), false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
}

TypeRegistry::~TypeRegistry()
{
    size_t size = types.size();
    for (size_t i = typeDeleteStartIndex; i < size; ++i)
    {
        delete types[i];
    }
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

    unsigned size = memberType->GetNumBits() * 2;
    uint16_t flags = TypeInfo::F_RANGE | (isHalfOpen ? TypeInfo::F_HALF_OPEN : TypeInfo::F_NONE);

    ROString name = compilerContext.stringBuilder
        .Append("Range")
        .Append(isHalfOpen ? "HalfOpen" : "Closed")
        .Append("'")
        .Append(memberType->GetName())
        .Append("'")
        .CreateString();

    TypeInfo* newRangeType = new TypeInfo(GetNextTypeId(), size, flags, TypeInfo::eNotApplicable, name, memberType);
    types.push_back(newRangeType);
    newRangeType->AddMember("Start", memberType, false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
    newRangeType->AddMember("End", memberType, false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
    rangeTypes.insert({key, newRangeType});

    return newRangeType;
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
        sb.Append(parameterTypes[0]->GetName());

        for (size_t i = 1; i < paramSize; ++i)
        {
            const TypeInfo* paramType = parameterTypes[i];

            sb.Append(", ");
            sb.Append(paramType->GetName());
        }
    }

    sb.Append(")");

    if (!returnType->IsUnit())
    {
        sb.Append(' ');
        sb.Append(returnType->GetName());
    }

    return sb.CreateString();
}

TypeInfo* TypeRegistry::GetStructType(ROString name, const Token* token)
{
    TypeInfo* newType = new TypeInfo(GetNextTypeId(), 0, TypeInfo::F_STRUCT, TypeInfo::eNotApplicable, name, nullptr, token);
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
    for (const Parameter* param : parameters)
    {
        parameterTypes.push_back(param->type);
    }

    ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, functionDeclaration->returnType);

    TypeInfo* newFunType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_FUNCTION, TypeInfo::eNotApplicable, name);

    types.push_back(newFunType);
    functionTypes.insert({key, newFunType});

    // add param and return types
    for (const Parameter* param : parameters)
    {
        newFunType->data->paramTypes.push_back(param->type);
        newFunType->data->paramNames.push_back(param->name);
    }
    newFunType->data->returnType = functionDeclaration->returnType;

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

    ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, returnType);

    TypeInfo* newFunType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_FUNCTION, TypeInfo::eNotApplicable, name);
    types.push_back(newFunType);
    functionTypes.insert({key, newFunType});

    // add param and return types
    size_t paramSize = parameterTypes.size();
    for (size_t i = 0; i < paramSize; ++i)
    {
        newFunType->data->paramTypes.push_back(parameterTypes[i]);
        newFunType->data->paramNames.push_back(parameterNames[i]);
    }
    newFunType->data->returnType = returnType;

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

    ROString name = compilerContext.stringBuilder
        .Append(POINTER_TYPE_TOKEN)
        .Append(type->GetName())
        .CreateString();
    TypeInfo* newPtrType = new TypeInfo(GetNextTypeId(), pointerSize, TypeInfo::F_POINTER, TypeInfo::eNotApplicable, name, type);
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

    ROString name = compilerContext.stringBuilder
        .Append(ARRAY_TYPE_START_TOKEN, ARRAY_TYPE_END_TOKEN, type->GetName())
        .CreateString();

    TypeInfo* newArrayType = new TypeInfo(GetNextTypeId(), pointerSize * 2, TypeInfo::F_ARRAY, TypeInfo::eNotApplicable, name, type);
    types.push_back(newArrayType);
    newArrayType->AddMember("Size", GetUIntSizeType(), false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
    newArrayType->AddMember("Data", GetPointerToType(type), false, MemberInfo::NO_DEFAULT_VALUE, Token::None);
    arrayTypes.insert({typeId, newArrayType});

    return newArrayType;
}

const TypeInfo* TypeRegistry::GetTypeAlias(ROString newName, const Token* newToken, const TypeInfo *typeInfo)
{
    const TypeInfo* newTypeInfo = new TypeInfo(newName, typeInfo->data, newToken);
    return newTypeInfo;
}

const NumericLiteralType* TypeRegistry::CreateSignedNumericLiteralType(unsigned numBits)
{
    return CreateNumericLiteralType(TypeInfo::eSigned, numBits, 0, "{signed-integer}");
}

const NumericLiteralType* TypeRegistry::CreateUnsignedNumericLiteralType(unsigned numBits)
{
    return CreateNumericLiteralType(TypeInfo::eUnsigned, 0, numBits, "{unsigned-integer}");
}

const NumericLiteralType* TypeRegistry::CreateContextDependentNumericLiteralType(unsigned signedNumBits, unsigned unsignedNumBits)
{
    return CreateNumericLiteralType(TypeInfo::eContextDependent, signedNumBits, unsignedNumBits, "{integer}");
}

const TypeInfo* TypeRegistry::CreateNumericLiteralType(TypeInfo::ESign sign, unsigned signedNumBits, unsigned unsignedNumBits)
{
    const TypeInfo* type = nullptr;
    switch (sign)
    {
        case TypeInfo::eNotApplicable:
            type = nullptr;
            break;
        case TypeInfo::eSigned:
            type = CreateSignedNumericLiteralType(signedNumBits);
            break;
        case TypeInfo::eUnsigned:
            type = CreateUnsignedNumericLiteralType(unsignedNumBits);
            break;
        case TypeInfo::eContextDependent:
            type = CreateContextDependentNumericLiteralType(signedNumBits, unsignedNumBits);
            break;
    }

    return type;
}

TypeId TypeRegistry::GetNextTypeId()
{
    return static_cast<TypeId>(types.size());
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
