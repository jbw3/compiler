#include "TypeInfo.h"

using namespace std;

TypeInfo boolTypeInfo(1, true, false);
TypeInfo int32TypeInfo(32, false, true);

const TypeInfo* TypeInfo::boolType = &boolTypeInfo;
const TypeInfo* TypeInfo::int32Type = &int32TypeInfo;

map<string, const TypeInfo*> TypeInfo::types = {
    {"bool", boolType},
    {"i32", int32Type},
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
    numBits(numBits),
    isBool(isBool),
    isInt(isInt)
{
}
