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

TypeRegistry::TypeRegistry(CompilerContext& compilerContext) :
    compilerContext(compilerContext)
{
    pointerSize = 8 * compilerContext.config.targetMachine->getAllocaPointerSize();

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

    intSizeType = new TypeInfo(pointerSize, TypeInfo::F_INT, TypeInfo::eSigned, INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    RegisterType(intSizeType);

    uintSizeType = new TypeInfo(pointerSize, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);

    stringType = new TypeInfo(pointerSize * 2, TypeInfo::F_STR | TypeInfo::F_AGGREGATE, TypeInfo::eNotApplicable, STR_KEYWORD, STR_KEYWORD);
    stringType->AddMember("Size", uintSizeType, false, Token::None);
    stringType->AddMember("Data", GetPointerToType(TypeInfo::UInt8Type), false, Token::None);
    RegisterType(stringType);
}

TypeRegistry::~TypeRegistry()
{
    // remove types that are not dynamically allocated
    types.erase(BOOL_KEYWORD);
    types.erase(INT8_KEYWORD);
    types.erase(INT16_KEYWORD);
    types.erase(INT32_KEYWORD);
    types.erase(INT64_KEYWORD);
    types.erase(UINT8_KEYWORD);
    types.erase(UINT16_KEYWORD);
    types.erase(UINT32_KEYWORD);
    types.erase(UINT64_KEYWORD);
    types.erase(FLOAT32_KEYWORD);
    types.erase(FLOAT64_KEYWORD);
    types.erase(TYPE_KEYWORD);

    // delete dynamically allocated types
    for (auto pair : types)
    {
        delete pair.second;
    }
    types.clear();

    for (auto pair : numericLiteralTypes)
    {
        delete pair.second;
    }
    numericLiteralTypes.clear();
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
    ROString uniqueName = compilerContext.stringBuilder
        .Append("Range")
        .Append(isHalfOpen ? "HalfOpen" : "Closed")
        .Append("'")
        .Append(memberType->GetUniqueName())
        .Append("'")
        .CreateString();
    const TypeInfo* rangeType = GetType(uniqueName);
    if (rangeType == nullptr)
    {
        unsigned size = memberType->GetNumBits() * 2;
        uint16_t flags = TypeInfo::F_RANGE | TypeInfo::F_AGGREGATE | (isHalfOpen ? TypeInfo::F_HALF_OPEN : TypeInfo::F_NONE);

        ROString name = compilerContext.stringBuilder
            .Append("Range")
            .Append(isHalfOpen ? "HalfOpen" : "Closed")
            .Append("'")
            .Append(memberType->GetShortName())
            .Append("'")
            .CreateString();

        TypeInfo* newRangeType = new TypeInfo(size, flags, TypeInfo::eNotApplicable, uniqueName, name, memberType);
        newRangeType->AddMember("Start", memberType, false, Token::None);
        newRangeType->AddMember("End", memberType, false, Token::None);

        rangeType = newRangeType;
    }

    return rangeType;
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

const TypeInfo* TypeRegistry::GetFunctionType(const FunctionDeclaration* functionDeclaration)
{
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

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, returnType);

        // add param and return types
        funType = TypeInfo::CreateFunctionType(GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, returnType);
        RegisterType(funType);
    }

    return funType;
}

const TypeInfo* TypeRegistry::GetFunctionType(
        const vector<const TypeInfo*>& parameterTypes,
        const vector<ROString>& parameterNames,
        const TypeInfo* returnType
)
{
    ROString uniqueName = getFunctionUniqueName(compilerContext.stringBuilder, parameterTypes, returnType);

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        ROString name = getFunctionName(compilerContext.stringBuilder, parameterTypes, returnType);
        funType = TypeInfo::CreateFunctionType(GetUIntSizeType()->GetNumBits(), uniqueName, name, parameterTypes, parameterNames, returnType);
        RegisterType(funType);
    }

    return funType;
}

const TypeInfo* TypeRegistry::GetPointerToType(const TypeInfo* type)
{
    ROString uniqueName = compilerContext.stringBuilder
        .Append(POINTER_TYPE_TOKEN)
        .Append(type->GetUniqueName())
        .CreateString();
    const TypeInfo* ptrType = GetType(uniqueName);
    if (ptrType == nullptr)
    {
        ROString name = compilerContext.stringBuilder
            .Append(POINTER_TYPE_TOKEN)
            .Append(type->GetShortName())
            .CreateString();
        TypeInfo* newPtrType = new TypeInfo(pointerSize, TypeInfo::F_POINTER, TypeInfo::eNotApplicable, uniqueName, name, type);
        RegisterType(newPtrType);

        ptrType = newPtrType;
    }

    return ptrType;
}

const TypeInfo* TypeRegistry::GetArrayOfType(const TypeInfo* type)
{
    ROString uniqueName = compilerContext.stringBuilder
        .Append(ARRAY_TYPE_START_TOKEN)
        .Append(ARRAY_TYPE_END_TOKEN)
        .Append(type->GetUniqueName())
        .CreateString();
    const TypeInfo* arrayType = GetType(uniqueName);
    if (arrayType == nullptr)
    {
        ROString name = compilerContext.stringBuilder
            .Append(ARRAY_TYPE_START_TOKEN, ARRAY_TYPE_END_TOKEN, type->GetShortName())
            .CreateString();

        TypeInfo* newArrayType = new TypeInfo(pointerSize * 2, TypeInfo::F_ARRAY, TypeInfo::eNotApplicable, uniqueName, name, type);
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

const TypeInfo* TypeRegistry::GetType(ROString typeName)
{
    auto iter = types.find(typeName);
    if (iter == types.cend())
    {
        return nullptr;
    }
    return iter->second;
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
            sign,
            signedNumBits,
            unsignedNumBits,
            GetNumericLiteralTypeUniqueName(signedNumBits, unsignedNumBits),
            name
        );
        numericLiteralTypes.insert({key, type});
    }
    else
    {
        type = iter->second;
    }

    return type;
}
