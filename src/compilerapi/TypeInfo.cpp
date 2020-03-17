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
PrimitiveType boolTypeInfo(1, true, false, false, BOOL_KEYWORD);
PrimitiveType int8TypeInfo(8, false, true, true, INT8_KEYWORD);
PrimitiveType int16TypeInfo(16, false, true, true, INT16_KEYWORD);
PrimitiveType int32TypeInfo(32, false, true, true, INT32_KEYWORD);
PrimitiveType int64TypeInfo(64, false, true, true, INT64_KEYWORD);
PrimitiveType uInt8TypeInfo(8, false, true, false, UINT8_KEYWORD);
PrimitiveType uInt16TypeInfo(16, false, true, false, UINT16_KEYWORD);
PrimitiveType uInt32TypeInfo(32, false, true, false, UINT32_KEYWORD);
PrimitiveType uInt64TypeInfo(64, false, true, false, UINT64_KEYWORD);

MemberInfo::MemberInfo(const string& name, const TypeInfo* type) :
    name(name),
    type(type)
{
}

const string& MemberInfo::GetName() const
{
    return name;
}

const TypeInfo* MemberInfo::GetType() const
{
    return type;
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

    intSizeType = new PrimitiveType(numBits, false, true, true, INT_SIZE_KEYWORD);
    RegisterType(INT_SIZE_KEYWORD, intSizeType);
    uintSizeType = new PrimitiveType(numBits, false, true, false, UINT_SIZE_KEYWORD);
    RegisterType(UINT_SIZE_KEYWORD, uintSizeType);

    stringPointerType = new StringPointerType(numBits);
    RegisterType(STR_KEYWORD, stringPointerType);
}

const TypeInfo* TypeInfo::GetIntSizeType()
{
    return intSizeType;
}

const TypeInfo* TypeInfo::GetUIntSizeType()
{
    return uintSizeType;
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

bool TypeInfo::RegisterType(const std::string& typeName, const TypeInfo* typeInfo)
{
    auto pair = types.insert({ typeName, typeInfo });
    return pair.second;
}

TypeInfo::TypeInfo(
    unsigned numBits,
    bool isBool,
    bool isInt,
    bool isSigned,
    const string& shortName
) :
    numBits(numBits),
    isBool(isBool),
    isInt(isInt),
    isSigned(isSigned),
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

bool TypeInfo::IsSigned() const
{
    return isSigned;
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

void TypeInfo::AddMember(const MemberInfo* member)
{
    members.insert({member->GetName(), member});
}

UnitTypeInfo::UnitTypeInfo() :
    TypeInfo(0, false, false, false, "Unit")
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
    bool isSigned,
    const string& shortName
) :
    TypeInfo(numBits, isBool, isInt, isSigned, shortName)
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
        && IsSigned() == primitiveOther.IsSigned();
}

StringPointerType::StringPointerType(unsigned numBits) :
    TypeInfo(numBits, false, false, false, STR_KEYWORD)
{
    AddMember(new MemberInfo("Size", TypeInfo::GetUIntSizeType()));
}

bool StringPointerType::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(StringPointerType);
    return isSame;
}
