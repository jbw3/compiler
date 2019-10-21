#include "TypeInfo.h"
#include <typeinfo>

using namespace std;

PrimitiveType boolTypeInfo(1, true, false, false);
PrimitiveType int8TypeInfo(8, false, true, true);
PrimitiveType int16TypeInfo(16, false, true, true);
PrimitiveType int32TypeInfo(32, false, true, true);
PrimitiveType int64TypeInfo(64, false, true, true);
PrimitiveType uInt8TypeInfo(8, false, true, false);
PrimitiveType uInt16TypeInfo(16, false, true, false);
PrimitiveType uInt32TypeInfo(32, false, true, false);
PrimitiveType uInt64TypeInfo(64, false, true, false);

const TypeInfo* TypeInfo::BoolType = &boolTypeInfo;
const TypeInfo* TypeInfo::Int8Type = &int8TypeInfo;
const TypeInfo* TypeInfo::Int16Type = &int16TypeInfo;
const TypeInfo* TypeInfo::Int32Type = &int32TypeInfo;
const TypeInfo* TypeInfo::Int64Type = &int64TypeInfo;
const TypeInfo* TypeInfo::UInt8Type = &uInt8TypeInfo;
const TypeInfo* TypeInfo::UInt16Type = &uInt16TypeInfo;
const TypeInfo* TypeInfo::UInt32Type = &uInt32TypeInfo;
const TypeInfo* TypeInfo::UInt64Type = &uInt64TypeInfo;

map<string, const TypeInfo*> TypeInfo::types =
{
    {"bool", BoolType},
    {"i8", Int8Type},
    {"i16", Int16Type},
    {"i32", Int32Type},
    {"i64", Int64Type},
    {"u8", UInt8Type},
    {"u16", UInt16Type},
    {"u32", UInt32Type},
    {"u64", UInt64Type},
};

const TypeInfo* TypeInfo::GetType(const string& typeName)
{
    auto iter = types.find(typeName);
    if (iter == types.cend())
    {
        return nullptr;
    }
    return iter->second;
}

TypeInfo::TypeInfo(
    unsigned numBits,
    bool isBool,
    bool isInt,
    bool isSigned
) :
    numBits(numBits),
    isBool(isBool),
    isInt(isInt),
    isSigned(isSigned)
{
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

PrimitiveType::PrimitiveType(
    unsigned numBits,
    bool isBool,
    bool isInt,
    bool isSigned
) :
    TypeInfo(numBits, isBool, isInt, isSigned)
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
