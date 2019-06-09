#include "TypeInfo.h"

using namespace std;

TypeInfo boolTypeInfo(1, true, false);
TypeInfo int8TypeInfo(8, false, true);
TypeInfo int16TypeInfo(16, false, true);
TypeInfo int32TypeInfo(32, false, true);
TypeInfo int64TypeInfo(64, false, true);

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
