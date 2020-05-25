#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
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

MemberInfo::MemberInfo(const string& name, unsigned index, const TypeInfo* type, bool isAssignable) :
    name(name),
    index(index),
    type(type),
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
    unsigned numBits = 8 * targetMachine->getAllocaPointerSize();

    intSizeType = new PrimitiveType(numBits, false, true, TypeInfo::eSigned, INT_SIZE_KEYWORD);
    RegisterType(intSizeType);
    uintSizeType = new PrimitiveType(numBits, false, true, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);

    stringPointerType = new StringPointerType(numBits);
    RegisterType(stringPointerType);
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
        delete iter->second;
    }

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
    auto iter = members.find(memberName);
    if (iter == members.cend())
    {
        return nullptr;
    }

    return iter->second;
}

const map<string, const MemberInfo*>& TypeInfo::GetMembers() const
{
    return members;
}

size_t TypeInfo::GetMemberCount() const
{
    return members.size();
}

bool TypeInfo::AddMember(const string& name, const TypeInfo* type, bool isAssignable)
{
    MemberInfo* member = new MemberInfo(name, members.size(), type, isAssignable);
    auto rv = members.insert({name, member});

    bool inserted = rv.second;
    if (!inserted)
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

const ContextInt* ContextInt::CreateSigned(unsigned numBits)
{
    // TODO: Fix memory leak
    const ContextInt* type = new ContextInt(
        numBits,
        TypeInfo::eSigned,
        numBits,
        0,
        "{signed-integer}"
    );
    return type;
}

const ContextInt* ContextInt::CreateUnsigned(unsigned numBits)
{
    // TODO: Fix memory leak
    const ContextInt* type = new ContextInt(
        numBits,
        TypeInfo::eUnsigned,
        0,
        numBits,
        "{unsigned-integer}"
    );
    return type;
}

ContextInt::ContextInt(unsigned signedNumBits, unsigned unsignedNumBits) :
    TypeInfo(0, false, true, TypeInfo::eContextDependent, false, "{integer}"),
    signedNumBits(signedNumBits),
    unsignedNumBits(unsignedNumBits)
{
}

ContextInt::ContextInt(
    unsigned numBits,
    ESign sign,
    unsigned signedNumBits,
    unsigned unsignedNumBits,
    const string& name
) :
    TypeInfo(numBits, false, true, sign, false, name),
    signedNumBits(signedNumBits),
    unsignedNumBits(unsignedNumBits)
{
}

bool ContextInt::IsSameAs(const TypeInfo& other) const
{
    if (typeid(other) != typeid(ContextInt))
    {
        return false;
    }

    const ContextInt& otherContextInt = static_cast<const ContextInt&>(other);
    return GetSign() == other.GetSign()
        && signedNumBits == otherContextInt.signedNumBits
        && unsignedNumBits == otherContextInt.unsignedNumBits;
}

unsigned ContextInt::GetNumBits() const
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

unsigned ContextInt::GetSignedNumBits() const
{
    return signedNumBits;
}

unsigned ContextInt::GetUnsignedNumBits() const
{
    return unsignedNumBits;
}

const TypeInfo* ContextInt::GetMinSizeType(ESign sign) const
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
    AddMember("Size", TypeInfo::GetUIntSizeType(), false);
}

bool StringPointerType::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(StringPointerType);
    return isSame;
}

AggregateType::AggregateType(const string& name) :
    TypeInfo(0, false, false, TypeInfo::eNotApplicable, true, name)
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
