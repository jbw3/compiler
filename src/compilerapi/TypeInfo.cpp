#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Token.h"
#include "TypeInfo.h"
#include "keywords.h"
#include "llvm/Target/TargetMachine.h"
#include <typeinfo>
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

UnitTypeInfo unitType;
PrimitiveType boolTypeInfo(1, true, false, TypeInfo::eNotApplicable, BOOL_KEYWORD);
PrimitiveType int8TypeInfo(8, false, true, TypeInfo::eSigned, INT8_KEYWORD);
PrimitiveType int16TypeInfo(16, false, true, TypeInfo::eSigned, INT16_KEYWORD);
PrimitiveType int32TypeInfo(32, false, true, TypeInfo::eSigned, INT32_KEYWORD);
PrimitiveType int64TypeInfo(64, false, true, TypeInfo::eSigned, INT64_KEYWORD);
PrimitiveType uInt8TypeInfo(8, false, true, TypeInfo::eUnsigned, UINT8_KEYWORD);
PrimitiveType uInt16TypeInfo(16, false, true, TypeInfo::eUnsigned, UINT16_KEYWORD);
PrimitiveType uInt32TypeInfo(32, false, true, TypeInfo::eUnsigned, UINT32_KEYWORD);
PrimitiveType uInt64TypeInfo(64, false, true, TypeInfo::eUnsigned, UINT64_KEYWORD);

MemberInfo::MemberInfo(const string& name, unsigned index, const TypeInfo* type, bool isAssignable, const Token* token) :
    name(name),
    index(index),
    type(type),
    token(token),
    isAssignable(isAssignable)
{
}

const string& MemberInfo::GetName() const
{
    return name;
}

unsigned MemberInfo::GetIndex() const
{
    return index;
}

const TypeInfo* MemberInfo::GetType() const
{
    return type;
}

bool MemberInfo::GetIsAssignable() const
{
    return isAssignable;
}

const Token* MemberInfo::GetToken() const
{
    return token;
}

const UnitTypeInfo* TypeInfo::UnitType = &unitType;
const TypeInfo* TypeInfo::BoolType = &boolTypeInfo;
const TypeInfo* TypeInfo::Int8Type = &int8TypeInfo;
const TypeInfo* TypeInfo::Int16Type = &int16TypeInfo;
const TypeInfo* TypeInfo::Int32Type = &int32TypeInfo;
const TypeInfo* TypeInfo::Int64Type = &int64TypeInfo;
const TypeInfo* TypeInfo::UInt8Type = &uInt8TypeInfo;
const TypeInfo* TypeInfo::UInt16Type = &uInt16TypeInfo;
const TypeInfo* TypeInfo::UInt32Type = &uInt32TypeInfo;
const TypeInfo* TypeInfo::UInt64Type = &uInt64TypeInfo;

unsigned TypeInfo::pointerSize = 0;
TypeInfo* TypeInfo::intSizeType = nullptr;
TypeInfo* TypeInfo::uintSizeType = nullptr;
TypeInfo* TypeInfo::stringPointerType = nullptr;

map<string, const TypeInfo*> TypeInfo::types =
{
    {BOOL_KEYWORD, BoolType},
    {INT8_KEYWORD, Int8Type},
    {INT16_KEYWORD, Int16Type},
    {INT32_KEYWORD, Int32Type},
    {INT64_KEYWORD, Int64Type},
    {UINT8_KEYWORD, UInt8Type},
    {UINT16_KEYWORD, UInt16Type},
    {UINT32_KEYWORD, UInt32Type},
    {UINT64_KEYWORD, UInt64Type},
};

void TypeInfo::InitTypes(const TargetMachine* targetMachine)
{
    pointerSize = 8 * targetMachine->getAllocaPointerSize();

    intSizeType = new PrimitiveType(pointerSize, false, true, TypeInfo::eSigned, INT_SIZE_KEYWORD);
    RegisterType(intSizeType);
    uintSizeType = new PrimitiveType(pointerSize, false, true, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);

    stringPointerType = new StringPointerType(pointerSize);
    RegisterType(stringPointerType);
}

unsigned TypeInfo::GetPointerSize()
{
    return pointerSize;
}

const TypeInfo* TypeInfo::GetIntSizeType()
{
    return intSizeType;
}

const TypeInfo* TypeInfo::GetUIntSizeType()
{
    return uintSizeType;
}

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

const TypeInfo* TypeInfo::GetStringPointerType()
{
    return stringPointerType;
}

const TypeInfo* TypeInfo::GetRangeType(const TypeInfo* memberType)
{
    // TODO: create a registry for range types instead of creating a new one each time
    const TypeInfo* rangeType = new RangeType(memberType);
    return rangeType;
}

const TypeInfo* TypeInfo::GetType(const string& typeName)
{
    auto iter = types.find(typeName);
    if (iter == types.cend())
    {
        return nullptr;
    }
    return iter->second;
}

bool TypeInfo::RegisterType(const TypeInfo* typeInfo)
{
    auto pair = types.insert({ typeInfo->GetShortName(), typeInfo });
    return pair.second;
}

TypeInfo::TypeInfo(
    unsigned numBits,
    bool isBool,
    bool isInt,
    ESign sign,
    bool isAggregate,
    const string& shortName
) :
    numBits(numBits),
    isBool(isBool),
    isInt(isInt),
    sign(sign),
    isAggregate(isAggregate),
    shortName(shortName)
{
}

TypeInfo::~TypeInfo()
{
    for (auto iter = members.begin(); iter != members.end(); ++iter)
    {
        delete *iter;
    }

    memberMap.clear();
    members.clear();
}

bool TypeInfo::IsBool() const
{
    return isBool;
}

bool TypeInfo::IsInt() const
{
    return isInt;
}

TypeInfo::ESign TypeInfo::GetSign() const
{
    return sign;
}

bool TypeInfo::IsAggregate() const
{
    return isAggregate;
}

unsigned TypeInfo::GetNumBits() const
{
    return numBits;
}

const string& TypeInfo::GetShortName() const
{
    return shortName;
}

const MemberInfo* TypeInfo::GetMember(const string& memberName) const
{
    auto iter = memberMap.find(memberName);
    if (iter == memberMap.cend())
    {
        return nullptr;
    }

    return iter->second;
}

const vector<const MemberInfo*>& TypeInfo::GetMembers() const
{
    return members;
}

size_t TypeInfo::GetMemberCount() const
{
    return members.size();
}

bool TypeInfo::AddMember(const string& name, const TypeInfo* type, bool isAssignable, const Token* token)
{
    MemberInfo* member = new MemberInfo(name, members.size(), type, isAssignable, token);
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

UnitTypeInfo::UnitTypeInfo() :
    TypeInfo(0, false, false, TypeInfo::eNotApplicable, false, "Unit")
{
}

bool UnitTypeInfo::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(UnitTypeInfo);
    return isSame;
}

PrimitiveType::PrimitiveType(
    unsigned numBits,
    bool isBool,
    bool isInt,
    ESign sign,
    const string& shortName
) :
    TypeInfo(numBits, isBool, isInt, sign, false, shortName)
{
}

bool PrimitiveType::IsSameAs(const TypeInfo& other) const
{
    if (typeid(other) != typeid(PrimitiveType))
    {
        return false;
    }

    const PrimitiveType& primitiveOther = static_cast<const PrimitiveType&>(other);
    return GetNumBits() == primitiveOther.GetNumBits()
        && IsBool() == primitiveOther.IsBool()
        && IsInt() == primitiveOther.IsInt()
        && GetSign() == primitiveOther.GetSign();
}

unordered_map
<
    tuple<TypeInfo::ESign, unsigned, unsigned>,
    const NumericLiteralType*
> NumericLiteralType::instances;

const NumericLiteralType* NumericLiteralType::Create(ESign sign, unsigned signedNumBits, unsigned unsignedNumBits, const char* name)
{
    const NumericLiteralType* type = nullptr;
    auto key = make_tuple(sign, signedNumBits, unsignedNumBits);

    auto iter = instances.find(key);
    if (iter == instances.end())
    {
        type = new NumericLiteralType(
            sign,
            signedNumBits,
            unsignedNumBits,
            name
        );
        instances.insert({key, type});
    }
    else
    {
        type = iter->second;
    }

    return type;
}

const NumericLiteralType* NumericLiteralType::Create(unsigned signedNumBits, unsigned unsignedNumBits)
{
    return Create(TypeInfo::eContextDependent, signedNumBits, unsignedNumBits, "{integer}");
}

const NumericLiteralType* NumericLiteralType::CreateSigned(unsigned numBits)
{
    return Create(TypeInfo::eSigned, numBits, 0, "{signed-integer}");
}

const NumericLiteralType* NumericLiteralType::CreateUnsigned(unsigned numBits)
{
    return Create(TypeInfo::eUnsigned, 0, numBits, "{unsigned-integer}");
}

NumericLiteralType::NumericLiteralType(
    ESign sign,
    unsigned signedNumBits,
    unsigned unsignedNumBits,
    const string& name
) :
    TypeInfo(0, false, true, sign, false, name),
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

const TypeInfo* NumericLiteralType::GetMinSizeType(ESign sign) const
{
    const TypeInfo* type = nullptr;
    switch (sign)
    {
        case TypeInfo::eNotApplicable:
            type = nullptr;
            break;
        case TypeInfo::eSigned:
            type = CreateSigned(signedNumBits);
            break;
        case TypeInfo::eUnsigned:
            type = CreateUnsigned(unsignedNumBits);
            break;
        case TypeInfo::eContextDependent:
            type = this;
            break;
    }

    return type;
}

StringPointerType::StringPointerType(unsigned numBits) :
    TypeInfo(numBits, false, false, TypeInfo::eNotApplicable, false, STR_KEYWORD)
{
    AddMember("Size", TypeInfo::GetUIntSizeType(), false, Token::None);
}

bool StringPointerType::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(StringPointerType);
    return isSame;
}

RangeType::RangeType(const TypeInfo* memberType) :
    TypeInfo(memberType->GetNumBits() * 2, false, false, TypeInfo::eNotApplicable, false, CreateRangeName(memberType))
{
    AddMember("Start", memberType, false, Token::None);
    AddMember("End", memberType, false, Token::None);
}

bool RangeType::IsSameAs(const TypeInfo& other) const
{
    const RangeType* otherRangeType = dynamic_cast<const RangeType*>(&other);
    if (otherRangeType == nullptr)
    {
        return false;
    }

    bool isSame = GetMembers()[0]->GetType()->IsSameAs(*otherRangeType->GetMembers()[0]->GetType());
    return isSame;
}

string RangeType::CreateRangeName(const TypeInfo* memberType)
{
    string memberName = memberType->GetShortName();
    string name = "Range'" + memberName + ", " + memberName + "'";
    return name;
}

AggregateType::AggregateType(const string& name, const Token* token) :
    TypeInfo(0, false, false, TypeInfo::eNotApplicable, true, name),
    token(token)
{
}

bool AggregateType::IsSameAs(const TypeInfo& other) const
{
    const AggregateType* otherAggType = dynamic_cast<const AggregateType*>(&other);
    if (otherAggType == nullptr)
    {
        return false;
    }

    bool isSame = GetShortName() == otherAggType->GetShortName();
    return isSame;
}

const Token* AggregateType::GetToken() const
{
    return token;
}
