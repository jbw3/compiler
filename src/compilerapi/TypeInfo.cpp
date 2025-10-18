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

TypeInfo unitType(0, 0, TypeInfo::F_UNIT, TypeInfo::eNotApplicable, "Unit");
TypeInfo boolTypeInfo(1, 1, TypeInfo::F_BOOL, TypeInfo::eNotApplicable, BOOL_KEYWORD);
TypeInfo int8TypeInfo(2, 8, TypeInfo::F_INT, TypeInfo::eSigned, INT8_KEYWORD);
TypeInfo int16TypeInfo(3, 16, TypeInfo::F_INT, TypeInfo::eSigned, INT16_KEYWORD);
TypeInfo int32TypeInfo(4, 32, TypeInfo::F_INT, TypeInfo::eSigned, INT32_KEYWORD);
TypeInfo int64TypeInfo(5, 64, TypeInfo::F_INT, TypeInfo::eSigned, INT64_KEYWORD);
TypeInfo uInt8TypeInfo(6, 8, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT8_KEYWORD);
TypeInfo uInt16TypeInfo(7, 16, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT16_KEYWORD);
TypeInfo uInt32TypeInfo(8, 32, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT32_KEYWORD);
TypeInfo uInt64TypeInfo(9, 64, TypeInfo::F_INT, TypeInfo::eUnsigned, UINT64_KEYWORD);
TypeInfo float32TypeInfo(10, 32, TypeInfo::F_FLOAT, TypeInfo::eSigned, FLOAT32_KEYWORD);
TypeInfo float64TypeInfo(11, 64, TypeInfo::F_FLOAT, TypeInfo::eSigned, FLOAT64_KEYWORD);
TypeInfo float32LiteralTypeInfo(12, 32, TypeInfo::F_FLOAT | TypeInfo::F_LITERAL, TypeInfo::eSigned, "{float-literal}");
TypeInfo float64LiteralTypeInfo(13, 64, TypeInfo::F_FLOAT | TypeInfo::F_LITERAL, TypeInfo::eSigned, "{float-literal}");
TypeInfo typeTypeInfo(14, 0, TypeInfo::F_TYPE, TypeInfo::eNotApplicable, TYPE_KEYWORD);

MemberInfo::MemberInfo(ROString name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token) :
    name(name),
    index(index),
    isStorage(isStorage),
    type(type),
    token(token)
{
}

const TypeInfo* TypeInfo::UnitType = &unitType;
const TypeInfo* TypeInfo::BoolType = &boolTypeInfo;
const TypeInfo* TypeInfo::Int8Type = &int8TypeInfo;
const TypeInfo* TypeInfo::Int16Type = &int16TypeInfo;
const TypeInfo* TypeInfo::Int32Type = &int32TypeInfo;
const TypeInfo* TypeInfo::Int64Type = &int64TypeInfo;
const TypeInfo* TypeInfo::UInt8Type = &uInt8TypeInfo;
const TypeInfo* TypeInfo::UInt16Type = &uInt16TypeInfo;
const TypeInfo* TypeInfo::UInt32Type = &uInt32TypeInfo;
const TypeInfo* TypeInfo::UInt64Type = &uInt64TypeInfo;
const TypeInfo* TypeInfo::Float32Type = &float32TypeInfo;
const TypeInfo* TypeInfo::Float64Type = &float64TypeInfo;
const TypeInfo* TypeInfo::Float32LiteralType = &float32LiteralTypeInfo;
const TypeInfo* TypeInfo::Float64LiteralType = &float64LiteralTypeInfo;
const TypeInfo* TypeInfo::TypeType = &typeTypeInfo;

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

TypeInfo::TypeInfo(
    TypeId id,
    unsigned numBits,
    uint16_t flags,
    ESign sign,
    ROString name,
    const TypeInfo* innerType,
    const Token* token
) :
    token(token),
    deleteData(true),
    name(name)
{
    data = new TypeInfoData;
    data->id = id;
    data->numBits = numBits;
    data->flags = flags;
    data->sign = sign;
    data->innerType = innerType;
    data->returnType = nullptr;
}

TypeInfo::TypeInfo(
    ROString name,
    TypeInfoData* data,
    const Token* token
) :
    token(token),
    deleteData(false),
    data(data),
    name(name)
{
}

TypeInfo::~TypeInfo()
{
    if (deleteData)
    {
        deletePointerContainer(data->members);
        delete data;
    }
}

bool TypeInfo::IsRecursiveStructDependency(ROString structName, vector<const Token*>& structTokenStack) const
{
    if (IsPointer())
    {
        return false;
    }
    else if (IsArray())
    {
        return data->innerType->IsRecursiveStructDependency(structName, structTokenStack);
    }
    else if (IsStruct())
    {
        structTokenStack.push_back(token);

        if (name == structName)
        {
            return true;
        }

        for (const MemberInfo* member : data->members)
        {
            if (member->GetType()->IsRecursiveStructDependency(structName, structTokenStack))
            {
                return true;
            }
        }

        return false;
    }
    else
    {
        return false;
    }
}

bool TypeInfo::IsOrContainsLiteral() const
{
    if (IsArray() || IsRange())
    {
        return data->innerType->IsOrContainsLiteral();
    }

    return IsLiteral();
}

bool TypeInfo::IsOrContainsNumericLiteral() const
{
    if (IsArray() || IsRange())
    {
        return data->innerType->IsOrContainsNumericLiteral();
    }

    return false;
}

const MemberInfo* TypeInfo::GetMember(ROString memberName) const
{
    auto iter = data->memberMap.find(memberName);
    if (iter == data->memberMap.cend())
    {
        return nullptr;
    }

    return iter->second;
}

bool TypeInfo::AddMember(ROString name, const TypeInfo* type, bool isAssignable, const Token* token)
{
    MemberInfo* member = new MemberInfo(name, static_cast<unsigned>(data->members.size()), type, isAssignable, token);
    auto rv = data->memberMap.insert({name, member});

    bool inserted = rv.second;
    if (inserted)
    {
        data->members.push_back(member);
    }
    else
    {
        delete member;
    }

    return inserted;
}

NumericLiteralType::NumericLiteralType(
    TypeId id,
    ESign sign,
    unsigned signedNumBits,
    unsigned unsignedNumBits,
    ROString name
) :
    TypeInfo(id, 0, F_INT | F_LITERAL, sign, name),
    signedNumBits(signedNumBits),
    unsignedNumBits(unsignedNumBits)
{
}

bool NumericLiteralType::IsOrContainsNumericLiteral() const
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
