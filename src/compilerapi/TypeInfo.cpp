#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "SyntaxTree.h"
#include "Token.h"
#include "TypeInfo.h"
#include "keywords.h"
#include "utils.h"
#include "llvm/Target/TargetMachine.h"
#include <typeinfo>
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

TypeInfo unitType(0, TypeInfo::F_UNIT, TypeInfo::eNotApplicable, "Unit", "Unit");
TypeInfo boolTypeInfo(1, TypeInfo::F_BOOL, TypeInfo::eNotApplicable, BOOL_KEYWORD, BOOL_KEYWORD);
TypeInfo int8TypeInfo(8, TypeInfo::F_INT, TypeInfo::eSigned, INT8_KEYWORD, INT8_KEYWORD);
TypeInfo int16TypeInfo(16, TypeInfo::F_INT, TypeInfo::eSigned, INT16_KEYWORD, INT16_KEYWORD);
TypeInfo int32TypeInfo(32, TypeInfo::F_INT, TypeInfo::eSigned, INT32_KEYWORD, INT32_KEYWORD);
TypeInfo int64TypeInfo(64, TypeInfo::F_INT, TypeInfo::eSigned, INT64_KEYWORD, INT64_KEYWORD);
TypeInfo uInt8TypeInfo(8, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT8_KEYWORD, UINT8_KEYWORD);
TypeInfo uInt16TypeInfo(16, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT16_KEYWORD, UINT16_KEYWORD);
TypeInfo uInt32TypeInfo(32, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT32_KEYWORD, UINT32_KEYWORD);
TypeInfo uInt64TypeInfo(64, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT64_KEYWORD, UINT64_KEYWORD);
TypeInfo float32TypeInfo(32, TypeInfo::F_FLOAT, TypeInfo::eSigned, FLOAT32_KEYWORD, FLOAT32_KEYWORD);
TypeInfo float64TypeInfo(64, TypeInfo::F_FLOAT, TypeInfo::eSigned, FLOAT64_KEYWORD, FLOAT64_KEYWORD);
TypeInfo float32LiteralTypeInfo(32, TypeInfo::F_FLOAT | TypeInfo::F_LITERAL, TypeInfo::eSigned, "{float32-literal}", "{float-literal}");
TypeInfo float64LiteralTypeInfo(64, TypeInfo::F_FLOAT | TypeInfo::F_LITERAL, TypeInfo::eSigned, "{float64-literal}", "{float-literal}");
TypeInfo typeTypeInfo(0, TypeInfo::F_TYPE, TypeInfo::eNotApplicable, TYPE_KEYWORD, TYPE_KEYWORD);

MemberInfo::MemberInfo(ROString name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token) :
    name(name),
    index(index),
    isStorage(isStorage),
    type(type),
    token(token)
{
}

const TypeInfo* TypeInfo::UnitType = &unitType;
const TypeInfo* TypeInfo::BoolType = &boolTypeInfo;
const TypeInfo* TypeInfo::Int8Type = &int8TypeInfo;
const TypeInfo* TypeInfo::Int16Type = &int16TypeInfo;
const TypeInfo* TypeInfo::Int32Type = &int32TypeInfo;
const TypeInfo* TypeInfo::Int64Type = &int64TypeInfo;
const TypeInfo* TypeInfo::UInt8Type = &uInt8TypeInfo;
const TypeInfo* TypeInfo::UInt16Type = &uInt16TypeInfo;
const TypeInfo* TypeInfo::UInt32Type = &uInt32TypeInfo;
const TypeInfo* TypeInfo::UInt64Type = &uInt64TypeInfo;
const TypeInfo* TypeInfo::Float32Type = &float32TypeInfo;
const TypeInfo* TypeInfo::Float64Type = &float64TypeInfo;
const TypeInfo* TypeInfo::Float32LiteralType = &float32LiteralTypeInfo;
const TypeInfo* TypeInfo::Float64LiteralType = &float64LiteralTypeInfo;
const TypeInfo* TypeInfo::TypeType = &typeTypeInfo;

const TypeInfo* TypeInfo::GetMinSignedIntTypeForSize(unsigned size)
{
    const TypeInfo* type = nullptr;

    if (size <= 8)
    {
        type = TypeInfo::Int8Type;
    }
    else if (size <= 16)
    {
        type = TypeInfo::Int16Type;
    }
    else if (size <= 32)
    {
        type = TypeInfo::Int32Type;
    }
    else if (size <= 64)
    {
        type = TypeInfo::Int64Type;
    }

    return type;
}

const TypeInfo* TypeInfo::GetMinUnsignedIntTypeForSize(unsigned size)
{
    const TypeInfo* type = nullptr;

    if (size <= 8)
    {
        type = TypeInfo::UInt8Type;
    }
    else if (size <= 16)
    {
        type = TypeInfo::UInt16Type;
    }
    else if (size <= 32)
    {
        type = TypeInfo::UInt32Type;
    }
    else if (size <= 64)
    {
        type = TypeInfo::UInt64Type;
    }

    return type;
}

TypeInfo* TypeInfo::CreateAggregateType(ROString name, const Token* token)
{
    return new TypeInfo(0, F_AGGREGATE, TypeInfo::eNotApplicable, name, name, nullptr, token);
}

const TypeInfo* TypeInfo::CreateFunctionType(
    unsigned numBits,
    ROString uniqueName,
    ROString name,
    const vector<const TypeInfo*>& parameterTypes,
    const vector<ROString>& parameterNames,
    const TypeInfo* returnType)
{
    TypeInfo* newFunType = new TypeInfo(numBits, TypeInfo::F_FUNCTION, TypeInfo::eNotApplicable, uniqueName, name);

    // add param and return types
    size_t paramSize = parameterTypes.size();
    for (size_t i = 0; i < paramSize; ++i)
    {
        newFunType->paramTypes.push_back(parameterTypes[i]);
        newFunType->paramNames.push_back(parameterNames[i]);
    }
    newFunType->returnType = returnType;

    return newFunType;
}

TypeInfo::TypeInfo(
    unsigned numBits,
    uint16_t flags,
    ESign sign,
    ROString uniqueName,
    ROString shortName,
    const TypeInfo* innerType,
    const Token* token
) :
    numBits(numBits),
    flags(flags),
    sign(sign),
    uniqueName(uniqueName),
    shortName(shortName),
    token(token),
    innerType(innerType),
    returnType(nullptr)
{
}

TypeInfo::~TypeInfo()
{
    deletePointerContainer(members);
    memberMap.clear();
}

bool pointersIsSameAs(const TypeInfo* type1, const TypeInfo* type2)
{
    if (type1 == nullptr || type2 == nullptr)
    {
        return type1 == type2;
    }
    else
    {
        return type1->IsSameAs(*type2);
    }
}

bool TypeInfo::IsSameAs(const TypeInfo& other) const
{
    const uint16_t CHECK_FLAGS = F_UNIT | F_BOOL | F_INT | F_FLOAT | F_STR | F_RANGE | F_POINTER | F_ARRAY | F_FUNCTION | F_TYPE | F_AGGREGATE | F_HALF_OPEN;
    bool isSame = numBits == other.numBits;
    isSame &= (flags & CHECK_FLAGS) == (other.flags & CHECK_FLAGS);
    isSame &= sign == other.sign;

    if (isSame)
    {
        isSame = shortName == other.shortName;
    }

    // compare inner types
    if (isSame)
    {
        isSame = pointersIsSameAs(innerType, other.innerType);
    }

    // compare param types
    if (isSame)
    {
        if (paramTypes.size() != other.paramTypes.size())
        {
            isSame = false;
        }
        else
        {
            size_t size = paramTypes.size();
            for (size_t i = 0; i < size; ++i)
            {
                if (!paramTypes[i]->IsSameAs(*other.paramTypes[i]))
                {
                    isSame = false;
                    break;
                }
            }
        }
    }

    // compare return types
    if (isSame)
    {
        isSame = pointersIsSameAs(returnType, other.returnType);
    }

    return isSame;
}

bool TypeInfo::IsOrContainsLiteral() const
{
    if (IsArray() || IsRange())
    {
        return innerType->IsOrContainsLiteral();
    }

    return IsLiteral();
}

bool TypeInfo::IsOrContainsNumericLiteral() const
{
    if (IsArray() || IsRange())
    {
        return innerType->IsOrContainsNumericLiteral();
    }

    return false;
}

const MemberInfo* TypeInfo::GetMember(ROString memberName) const
{
    auto iter = memberMap.find(memberName);
    if (iter == memberMap.cend())
    {
        return nullptr;
    }

    return iter->second;
}

bool TypeInfo::AddMember(ROString name, const TypeInfo* type, bool isAssignable, const Token* token)
{
    MemberInfo* member = new MemberInfo(name, static_cast<unsigned>(members.size()), type, isAssignable, token);
    auto rv = memberMap.insert({name, member});

    bool inserted = rv.second;
    if (inserted)
    {
        members.push_back(member);
    }
    else
    {
        delete member;
    }

    return inserted;
}

NumericLiteralType::NumericLiteralType(
    ESign sign,
    unsigned signedNumBits,
    unsigned unsignedNumBits,
    ROString uniqueName,
    ROString name
) :
    TypeInfo(0, F_INT, sign, uniqueName, name),
    signedNumBits(signedNumBits),
    unsignedNumBits(unsignedNumBits)
{
}

bool NumericLiteralType::IsSameAs(const TypeInfo& other) const
{
    if (typeid(other) != typeid(NumericLiteralType))
    {
        return false;
    }

    const NumericLiteralType& otherLiteralType = static_cast<const NumericLiteralType&>(other);
    return GetSign() == other.GetSign()
        && signedNumBits == otherLiteralType.signedNumBits
        && unsignedNumBits == otherLiteralType.unsignedNumBits;
}

bool NumericLiteralType::IsOrContainsNumericLiteral() const
{
    return true;
}

unsigned NumericLiteralType::GetNumBits() const
{
    if (GetSign() == eSigned)
    {
        return signedNumBits;
    }
    else
    {
        return unsignedNumBits;
    }
}

unsigned NumericLiteralType::GetSignedNumBits() const
{
    return signedNumBits;
}

unsigned NumericLiteralType::GetUnsignedNumBits() const
{
    return unsignedNumBits;
}
