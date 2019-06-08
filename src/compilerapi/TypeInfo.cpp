#include "TypeInfo.h"

using namespace std;

TypeInfo boolTypeInfo(1, true, false);
TypeInfo int32TypeInfo(32, false, true);

map<string, const TypeInfo*> TypeInfo::types = {
    {"bool", &boolTypeInfo},
    {"i32", &int32TypeInfo},
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
