#include "TypeInfo.h"
#include <typeinfo>

using namespace std;

PrimitiveType boolTypeInfo(1, true, false);
PrimitiveType int8TypeInfo(8, false, true);
PrimitiveType int16TypeInfo(16, false, true);
PrimitiveType int32TypeInfo(32, false, true);
PrimitiveType int64TypeInfo(64, false, true);

const TypeInfo* TypeInfo::BoolType = &boolTypeInfo;
const TypeInfo* TypeInfo::Int8Type = &int8TypeInfo;
const TypeInfo* TypeInfo::Int16Type = &int16TypeInfo;
const TypeInfo* TypeInfo::Int32Type = &int32TypeInfo;
const TypeInfo* TypeInfo::Int64Type = &int64TypeInfo;

map<string, const TypeInfo*> TypeInfo::types = {
    {"bool", BoolType},
    {"i8", Int8Type},
    {"i16", Int16Type},
    {"i32", Int32Type},
    {"i64", Int64Type},
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
    bool isInt
) :
    NumBits(numBits),
    IsBool(isBool),
    IsInt(isInt)
{
}

PrimitiveType::PrimitiveType(
    unsigned numBits,
    bool isBool,
    bool isInt
) :
    TypeInfo(numBits, isBool, isInt)
{
}

bool PrimitiveType::IsSameAs(const TypeInfo& other) const
{
    if (typeid(other) != typeid(PrimitiveType))
    {
        return false;
    }

    const PrimitiveType& primitiveOther = static_cast<const PrimitiveType&>(other);
    return NumBits == primitiveOther.NumBits
        && IsBool == primitiveOther.IsBool
        && IsInt == primitiveOther.IsInt;
}
