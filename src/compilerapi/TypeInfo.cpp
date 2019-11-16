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
PrimitiveType boolTypeInfo(1, true, false, false);
PrimitiveType int8TypeInfo(8, false, true, true);
PrimitiveType int16TypeInfo(16, false, true, true);
PrimitiveType int32TypeInfo(32, false, true, true);
PrimitiveType int64TypeInfo(64, false, true, true);
PrimitiveType uInt8TypeInfo(8, false, true, false);
PrimitiveType uInt16TypeInfo(16, false, true, false);
PrimitiveType uInt32TypeInfo(32, false, true, false);
PrimitiveType uInt64TypeInfo(64, false, true, false);

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

    RegisterType(INT_SIZE_KEYWORD, new PrimitiveType(numBits, false, true, true));
    RegisterType(UINT_SIZE_KEYWORD, new PrimitiveType(numBits, false, true, false));
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

UnitTypeInfo::UnitTypeInfo() :
    TypeInfo(0, false, false, false)
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
