#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "SyntaxTree.h"
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
using namespace SyntaxTree;

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
PrimitiveType typeTypeInfo(0, TypeInfo::F_TYPE, TypeInfo::eNotApplicable, "type", "type"); // TODO: use constant for type name

PrimitiveType immutBoolTypeInfo(1, TypeInfo::F_BOOL | TypeInfo::F_IMMUTABLE, TypeInfo::eNotApplicable, "immutable_"s + BOOL_KEYWORD, BOOL_KEYWORD);
PrimitiveType immutInt8TypeInfo(8, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_"s + INT8_KEYWORD, INT8_KEYWORD);
PrimitiveType immutInt16TypeInfo(16, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_"s + INT16_KEYWORD, INT16_KEYWORD);
PrimitiveType immutInt32TypeInfo(32, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_"s + INT32_KEYWORD, INT32_KEYWORD);
PrimitiveType immutInt64TypeInfo(64, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eSigned, "immutable_"s + INT64_KEYWORD, INT64_KEYWORD);
PrimitiveType immutUInt8TypeInfo(8, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_"s + UINT8_KEYWORD, UINT8_KEYWORD);
PrimitiveType immutUInt16TypeInfo(16, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_"s + UINT16_KEYWORD, UINT16_KEYWORD);
PrimitiveType immutUInt32TypeInfo(32, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_"s + UINT32_KEYWORD, UINT32_KEYWORD);
PrimitiveType immutUInt64TypeInfo(64, TypeInfo::F_INT | TypeInfo::F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_"s + UINT64_KEYWORD, UINT64_KEYWORD);

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
const TypeInfo* TypeInfo::TypeType = &typeTypeInfo;
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
    PrimitiveType* immutIntSizeType = new PrimitiveType(pointerSize, F_INT | F_IMMUTABLE, TypeInfo::eSigned, "immutable_"s + INT_SIZE_KEYWORD, INT_SIZE_KEYWORD);
    immutableTypes.insert({INT_SIZE_KEYWORD, immutIntSizeType});

    uintSizeType = new PrimitiveType(pointerSize, F_INT, TypeInfo::eUnsigned, UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
    RegisterType(uintSizeType);
    PrimitiveType* immutUIntSizeType = new PrimitiveType(pointerSize, F_INT | F_IMMUTABLE, TypeInfo::eUnsigned, "immutable_"s + UINT_SIZE_KEYWORD, UINT_SIZE_KEYWORD);
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

const TypeInfo* TypeInfo::GetRangeType(const TypeInfo* memberType, bool isHalfOpen)
{
    string uniqueName = "Range";
    uniqueName += (isHalfOpen ? "HalfOpen" : "Closed");
    uniqueName += "'" + memberType->GetUniqueName() + "'";
    const TypeInfo* rangeType = GetType(uniqueName);
    if (rangeType == nullptr)
    {
        unsigned size = memberType->GetNumBits() * 2;
        uint16_t flags = F_RANGE | F_AGGREGATE | (isHalfOpen ? F_HALF_OPEN : F_NONE);

        string name = "Range";
        name += (isHalfOpen ? "HalfOpen" : "Closed");
        name += "'" + memberType->GetShortName() + "'";

        TypeInfo* newRangeType = new PrimitiveType(size, flags, TypeInfo::eNotApplicable, uniqueName, name);
        newRangeType->innerType = memberType;
        newRangeType->AddMember("Start", memberType, false, Token::None);
        newRangeType->AddMember("End", memberType, false, Token::None);

        rangeType = newRangeType;
    }

    return rangeType;
}

const TypeInfo* TypeInfo::GetFunctionType(const FunctionDeclaration* functionDeclaration)
{
    const Parameters& parameters = functionDeclaration->parameters;
    string uniqueName = "fun(";
    string name = "fun(";
    if (parameters.size() > 0)
    {
        uniqueName += parameters[0]->type->GetUniqueName();
        name += parameters[0]->type->GetShortName();

        for (size_t i = 0; i < parameters.size(); ++i)
        {
            const TypeInfo* paramType = parameters[i]->type;

            uniqueName += ", ";
            uniqueName += paramType->GetUniqueName();

            name += ", ";
            name += paramType->GetShortName();
        }
    }

    uniqueName += ")";
    name += ")";

    uniqueName += functionDeclaration->returnType->GetUniqueName();
    if (!functionDeclaration->returnType->IsUnit())
    {
        name += ' ';
        name += functionDeclaration->returnType->GetShortName();
    }

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        TypeInfo* newFunType = new PrimitiveType(GetUIntSizeType()->GetNumBits(), F_FUNCTION, TypeInfo::eNotApplicable, uniqueName, name);

        // add param and return types
        for (const Parameter* param : parameters)
        {
            newFunType->paramTypes.push_back(param->type);
            newFunType->paramNames.push_back(param->name);
        }
        newFunType->returnType = functionDeclaration->returnType;

        funType = newFunType;
    }

    return funType;
}

const TypeInfo* TypeInfo::GetFunctionType(
        const vector<const TypeInfo*>& parameterTypes,
        const vector<string>& parameterNames,
        const TypeInfo* returnType
)
{
    size_t paramSize = parameterTypes.size();
    string uniqueName = "fun(";
    string name = "fun(";
    if (paramSize > 0)
    {
        uniqueName += parameterTypes[0]->GetUniqueName();
        name += parameterTypes[0]->GetShortName();

        for (size_t i = 0; i < paramSize; ++i)
        {
            const TypeInfo* paramType = parameterTypes[i];

            uniqueName += ", ";
            uniqueName += paramType->GetUniqueName();

            name += ", ";
            name += paramType->GetShortName();
        }
    }

    uniqueName += ")";
    name += ")";

    uniqueName += returnType->GetUniqueName();
    if (!returnType->IsUnit())
    {
        name += ' ';
        name += returnType->GetShortName();
    }

    const TypeInfo* funType = GetType(uniqueName);
    if (funType == nullptr)
    {
        TypeInfo* newFunType = new PrimitiveType(GetUIntSizeType()->GetNumBits(), F_FUNCTION, TypeInfo::eNotApplicable, uniqueName, name);

        // add param and return types
        for (size_t i = 0; i < paramSize; ++i)
        {
            newFunType->paramTypes.push_back(parameterTypes[i]);
            newFunType->paramNames.push_back(parameterNames[i]);
        }
        newFunType->returnType = returnType;

        funType = newFunType;
    }

    return funType;

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
    innerType(nullptr),
    returnType(nullptr)
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

bool TypeInfo::IsFunction() const
{
    return (flags & F_FUNCTION) != 0;
}

bool TypeInfo::IsType() const
{
    return (flags & F_TYPE) != 0;
}

TypeInfo::ESign TypeInfo::GetSign() const
{
    return sign;
}

bool TypeInfo::IsAggregate() const
{
    return (flags & F_AGGREGATE) != 0;
}

bool TypeInfo::IsHalfOpen() const
{
    return (flags & F_HALF_OPEN) != 0;
}

bool TypeInfo::IsImmutable() const
{
    return (flags & F_IMMUTABLE) != 0;
}

bool TypeInfo::IsNumericLiteral() const
{
    if (IsArray() || IsRange())
    {
        return innerType->IsNumericLiteral();
    }

    return false;
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

const vector<const TypeInfo*>& TypeInfo::GetParamTypes() const
{
    return paramTypes;
}

const vector<string>& TypeInfo::GetParamNames() const
{
    return paramNames;
}

const TypeInfo* TypeInfo::GetReturnType() const
{
    return returnType;
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

bool pointersIsSameAs(const TypeInfo* type1, const TypeInfo* type2)
{
    if (type1 == nullptr || type2 == nullptr)
    {
        return type1 == type2;
    }
    else
    {
        return type1->IsSameAs(*type2);
    }
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
        && IsArray() == primitiveOther.IsArray()
        && IsFunction() == primitiveOther.IsFunction()
        && GetSign() == primitiveOther.GetSign();

    // compare inner types
    if (isSame)
    {
        isSame = pointersIsSameAs(GetInnerType(), primitiveOther.GetInnerType());
    }

    // compare param types
    if (isSame)
    {
        const vector<const TypeInfo*>& myParamTypes = GetParamTypes();
        const vector<const TypeInfo*>& otherParamTypes = primitiveOther.GetParamTypes();
        if (myParamTypes.size() != otherParamTypes.size())
        {
            isSame = false;
        }
        else
        {
            size_t size = myParamTypes.size();
            for (size_t i = 0; i < size; ++i)
            {
                if (!myParamTypes[i]->IsSameAs(*otherParamTypes[i]))
                {
                    isSame = false;
                    break;
                }
            }
        }
    }

    // compare return types
    if (isSame)
    {
        isSame = pointersIsSameAs(GetReturnType(), primitiveOther.GetReturnType());
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

bool NumericLiteralType::IsNumericLiteral() const
{
    return true;
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
