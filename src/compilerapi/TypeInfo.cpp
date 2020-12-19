#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
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

UnitTypeInfo unitType;
PrimitiveType boolTypeInfo(1, TypeInfo::F_BOOL, TypeInfo::eNotApplicable, BOOL_KEYWORD, BOOL_KEYWORD);
PrimitiveType int8TypeInfo(8, TypeInfo::F_INT, TypeInfo::eSigned, INT8_KEYWORD, INT8_KEYWORD);
PrimitiveType int16TypeInfo(16, TypeInfo::F_INT, TypeInfo::eSigned, INT16_KEYWORD, INT16_KEYWORD);
PrimitiveType int32TypeInfo(32, TypeInfo::F_INT, TypeInfo::eSigned, INT32_KEYWORD, INT32_KEYWORD);
PrimitiveType int64TypeInfo(64, TypeInfo::F_INT, TypeInfo::eSigned, INT64_KEYWORD, INT64_KEYWORD);
PrimitiveType uInt8TypeInfo(8, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT8_KEYWORD, UINT8_KEYWORD);
PrimitiveType uInt16TypeInfo(16, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT16_KEYWORD, UINT16_KEYWORD);
PrimitiveType uInt32TypeInfo(32, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT32_KEYWORD, UINT32_KEYWORD);
PrimitiveType uInt64TypeInfo(64, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT64_KEYWORD, UINT64_KEYWORD);

PrimitiveType immutBoolTypeInfo(1, TypeInfo::F_BOOL | TypeInfo::F_IMMUTABLE, TypeInfo::eNotApplicable, "immutable_" + BOOL_KEYWORD, BOOL_KEYWORD);
PrimitiveType immutInt8TypeInfo(8, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_" + INT8_KEYWORD, INT8_KEYWORD);
PrimitiveType immutInt16TypeInfo(16, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_" + INT16_KEYWORD, INT16_KEYWORD);
PrimitiveType immutInt32TypeInfo(32, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_" + INT32_KEYWORD, INT32_KEYWORD);
PrimitiveType immutInt64TypeInfo(64, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_" + INT64_KEYWORD, INT64_KEYWORD);
PrimitiveType immutUInt8TypeInfo(8, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_" + UINT8_KEYWORD, UINT8_KEYWORD);
PrimitiveType immutUInt16TypeInfo(16, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_" + UINT16_KEYWORD, UINT16_KEYWORD);
PrimitiveType immutUInt32TypeInfo(32, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_" + UINT32_KEYWORD, UINT32_KEYWORD);
PrimitiveType immutUInt64TypeInfo(64, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_" + UINT64_KEYWORD, UINT64_KEYWORD);

MemberInfo::MemberInfo(const string& name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token) :
    name(name),
    index(index),
    type(type),
    token(token),
    isStorage(isStorage)
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

bool MemberInfo::GetIsStorage() const
{
    return isStorage;
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
const TypeInfo* TypeInfo::ImmutBoolType = &immutBoolTypeInfo;
const TypeInfo* TypeInfo::ImmutInt8Type = &immutInt8TypeInfo;
const TypeInfo* TypeInfo::ImmutInt16Type = &immutInt16TypeInfo;
const TypeInfo* TypeInfo::ImmutInt32Type = &immutInt32TypeInfo;
const TypeInfo* TypeInfo::ImmutInt64Type = &immutInt64TypeInfo;
const TypeInfo* TypeInfo::ImmutUInt8Type = &immutUInt8TypeInfo;
const TypeInfo* TypeInfo::ImmutUInt16Type = &immutUInt16TypeInfo;
const TypeInfo* TypeInfo::ImmutUInt32Type = &immutUInt32TypeInfo;
const TypeInfo* TypeInfo::ImmutUInt64Type = &immutUInt64TypeInfo;

unsigned TypeInfo::pointerSize = 0;
TypeInfo* TypeInfo::intSizeType = nullptr;
TypeInfo* TypeInfo::uintSizeType = nullptr;
TypeInfo* TypeInfo::stringType = nullptr;

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

unordered_map<string, const TypeInfo*> TypeInfo::immutableTypes =
{
    {BOOL_KEYWORD, ImmutBoolType},
    {INT8_KEYWORD, ImmutInt8Type},
    {INT16_KEYWORD, ImmutInt16Type},
    {INT32_KEYWORD, ImmutInt32Type},
    {INT64_KEYWORD, ImmutInt64Type},
    {UINT8_KEYWORD, ImmutUInt8Type},
    {UINT16_KEYWORD, ImmutUInt16Type},
    {UINT32_KEYWORD, ImmutUInt32Type},
    {UINT64_KEYWORD, ImmutUInt64Type},
};

void TypeInfo::InitTypes(const TargetMachine* targetMachine)
{
    pointerSize = 8 * targetMachine->getAllocaPointerSize();

    intSizeType = new PrimitiveType(pointerSize, F_INT, TypeInfo::eSigned, INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    RegisterType(intSizeType);
    PrimitiveType* immutIntSizeType = new PrimitiveType(pointerSize, F_INT | F_IMMUTABLE, TypeInfo::eSigned, "immutable_" + INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    immutableTypes.insert({INT_SIZE_KEYWORD, immutIntSizeType});

    uintSizeType = new PrimitiveType(pointerSize, F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);
    PrimitiveType* immutUIntSizeType = new PrimitiveType(pointerSize, F_INT | F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_" + UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    immutableTypes.insert({UINT_SIZE_KEYWORD, immutUIntSizeType});

    stringType = new StringType(pointerSize * 2);
    RegisterType(stringType);
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

const TypeInfo* TypeInfo::GetStringType()
{
    return stringType;
}

const TypeInfo* TypeInfo::GetRangeType(const TypeInfo* memberType, bool isExclusive)
{
    // TODO: create a registry for range types instead of creating a new one each time
    const TypeInfo* rangeType = new RangeType(memberType, isExclusive);
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
    auto pair = types.insert({ typeInfo->GetUniqueName(), typeInfo });
    return pair.second;
}

const TypeInfo* TypeInfo::GetPointerToType(const TypeInfo* type)
{
    string uniqueName = POINTER_TYPE_TOKEN + type->GetUniqueName();
    const TypeInfo* ptrType = GetType(uniqueName);
    if (ptrType == nullptr)
    {
        string name = POINTER_TYPE_TOKEN + type->GetShortName();
        TypeInfo* newPtrType = new PrimitiveType(pointerSize, F_POINTER, eNotApplicable, uniqueName, name);
        newPtrType->innerType = type;
        RegisterType(newPtrType);

        ptrType = newPtrType;
    }

    return ptrType;
}

const TypeInfo* TypeInfo::GetArrayOfType(const TypeInfo* type)
{
    string uniqueName;
    uniqueName += ARRAY_TYPE_START_TOKEN;
    uniqueName += type->GetUniqueName();
    uniqueName += ARRAY_TYPE_END_TOKEN;
    const TypeInfo* arrayType = GetType(uniqueName);
    if (arrayType == nullptr)
    {
        string name;
        name += ARRAY_TYPE_START_TOKEN;
        name += type->GetShortName();
        name += ARRAY_TYPE_END_TOKEN;

        TypeInfo* newArrayType = new PrimitiveType(pointerSize * 2, F_ARRAY, eNotApplicable, uniqueName, name);
        newArrayType->innerType = type;
        newArrayType->AddMember("Size", TypeInfo::GetUIntSizeType(), false, Token::None);
        newArrayType->AddMember("Data", TypeInfo::GetPointerToType(type), false, Token::None);
        RegisterType(newArrayType);

        arrayType = newArrayType;
    }

    return arrayType;
}

TypeInfo::TypeInfo(
    unsigned numBits,
    uint16_t flags,
    ESign sign,
    const string& uniqueName,
    const string& shortName
) :
    numBits(numBits),
    flags(flags),
    sign(sign),
    uniqueName(uniqueName),
    shortName(shortName),
    innerType(nullptr)
{
}

TypeInfo::~TypeInfo()
{
    deletePointerContainer(members);
    memberMap.clear();
}

uint16_t TypeInfo::GetFlags() const
{
    return flags;
}

bool TypeInfo::IsUnit() const
{
    return (flags & F_UNIT) != 0;
}

bool TypeInfo::IsBool() const
{
    return (flags & F_BOOL) != 0;
}

bool TypeInfo::IsInt() const
{
    return (flags & F_INT) != 0;
}

bool TypeInfo::IsRange() const
{
    return (flags & F_RANGE) != 0;
}

bool TypeInfo::IsPointer() const
{
    return (flags & F_POINTER) != 0;
}

bool TypeInfo::IsArray() const
{
    return (flags & F_ARRAY) != 0;
}

TypeInfo::ESign TypeInfo::GetSign() const
{
    return sign;
}

bool TypeInfo::IsAggregate() const
{
    return (flags & F_AGGREGATE) != 0;
}

bool TypeInfo::IsExclusive() const
{
    return (flags & F_EXCLUSIVE) != 0;
}

bool TypeInfo::IsImmutable() const
{
    return (flags & F_IMMUTABLE) != 0;
}

unsigned TypeInfo::GetNumBits() const
{
    return numBits;
}

const string& TypeInfo::GetUniqueName() const
{
    return uniqueName;
}

const string& TypeInfo::GetShortName() const
{
    return shortName;
}

const TypeInfo* TypeInfo::GetImmutableType() const
{
    // TODO: immutableTypes does not have all types. Need a better way to do this
    auto iter = immutableTypes.find(shortName);
    if (iter == immutableTypes.cend())
    {
        return nullptr;
    }

    return iter->second;
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

const TypeInfo* TypeInfo::GetInnerType() const
{
    return innerType;
}

UnitTypeInfo::UnitTypeInfo() :
    TypeInfo(0, F_UNIT, TypeInfo::eNotApplicable, "Unit", "Unit")
{
}

bool UnitTypeInfo::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(UnitTypeInfo);
    return isSame;
}

PrimitiveType::PrimitiveType(
    unsigned numBits,
    uint16_t flags,
    ESign sign,
    const string& uniqueName,
    const string& shortName
) :
    TypeInfo(numBits, flags, sign, uniqueName, shortName)
{
}

bool PrimitiveType::IsSameAs(const TypeInfo& other) const
{
    if (typeid(other) != typeid(PrimitiveType))
    {
        return false;
    }

    const PrimitiveType& primitiveOther = static_cast<const PrimitiveType&>(other);
    bool isSame =
        GetNumBits() == primitiveOther.GetNumBits()
        && IsBool() == primitiveOther.IsBool()
        && IsInt() == primitiveOther.IsInt()
        && IsPointer() == primitiveOther.IsPointer()
        && GetSign() == primitiveOther.GetSign();

    if (isSame)
    {
        const TypeInfo* inner = GetInnerType();
        const TypeInfo* otherInner = primitiveOther.GetInnerType();
        if (inner != otherInner)
        {
            if (inner == nullptr || otherInner == nullptr)
            {
                isSame = false;
            }
            else
            {
                isSame = inner->IsSameAs(*otherInner);
            }
        }
    }

    return isSame;
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

string GetNumericLiteralTypeUniqueName(unsigned signedNumBits, unsigned unsignedNumBits)
{
    string name;
    name += "{integer-";
    name += to_string(signedNumBits);
    name += "-";
    name += to_string(unsignedNumBits);
    name += "}";

    return name;
}

NumericLiteralType::NumericLiteralType(
    ESign sign,
    unsigned signedNumBits,
    unsigned unsignedNumBits,
    const string& name
) :
    TypeInfo(0, F_INT, sign, GetNumericLiteralTypeUniqueName(signedNumBits, unsignedNumBits), name),
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

StringType::StringType(unsigned numBits) :
    TypeInfo(numBits, F_AGGREGATE, TypeInfo::eNotApplicable, STR_KEYWORD, STR_KEYWORD)
{
    AddMember("Size", TypeInfo::GetUIntSizeType(), false, Token::None);
    AddMember("Data", TypeInfo::GetPointerToType(TypeInfo::UInt8Type), false, Token::None);
}

bool StringType::IsSameAs(const TypeInfo& other) const
{
    bool isSame = typeid(other) == typeid(StringType);
    return isSame;
}

string CreateUniqueRangeName(const TypeInfo* memberType, bool isExclusive)
{
    string memberUniqueName = memberType->GetUniqueName();
    string name = "Range";
    name += (isExclusive ? "Exclusive" : "Inclusive");
    name += "'" + memberUniqueName + "'";
    return name;
}

string CreateRangeName(const TypeInfo* memberType, bool isExclusive)
{
    string memberName = memberType->GetShortName();
    string name = "Range";
    name += (isExclusive ? "Exclusive" : "Inclusive");
    name += "'" + memberName + "'";
    return name;
}

RangeType::RangeType(const TypeInfo* memberType, bool isExclusive) :
    TypeInfo(
        memberType->GetNumBits() * 2,
        F_RANGE | F_AGGREGATE | (isExclusive ? F_EXCLUSIVE : F_NONE),
        TypeInfo::eNotApplicable,
        CreateUniqueRangeName(memberType, isExclusive),
        CreateRangeName(memberType, isExclusive)
    )
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

    bool isSame = GetMembers()[0]->GetType()->IsSameAs(*otherRangeType->GetMembers()[0]->GetType())
               && GetFlags() == other.GetFlags();
    return isSame;
}

AggregateType::AggregateType(const string& name, const Token* token) :
    TypeInfo(0, F_AGGREGATE, TypeInfo::eNotApplicable, name, name),
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
