#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include "ROString.h"
#include <unordered_map>
#include <vector>

namespace llvm
{
class TargetMachine;
}
namespace SyntaxTree
{
class FunctionDeclaration;
}
class Token;
class TypeInfo;

const char* const POINTER_TYPE_TOKEN = "&";
const char* const DOUBLE_POINTER_TYPE_TOKEN = "&&";
const char* const ARRAY_TYPE_START_TOKEN = "[";
const char* const ARRAY_TYPE_END_TOKEN = "]";

class MemberInfo
{
public:
    MemberInfo(ROString name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token);

    const ROString GetName() const
    {
        return name;
    }

    unsigned GetIndex() const
    {
        return index;
    }

    const TypeInfo* GetType() const
    {
        return type;
    }

    bool GetIsStorage() const
    {
        return isStorage;
    }

    const Token* GetToken() const
    {
        return token;
    }

private:
    ROString name;
    unsigned index;
    bool isStorage;
    const TypeInfo* type;
    const Token* token;
};

typedef uint32_t TypeId;

class TypeInfo
{
public:
    static constexpr uint16_t F_NONE = 0;

    // main types
    static constexpr uint16_t F_UNIT     = 1 << 0;
    static constexpr uint16_t F_BOOL     = 1 << 1;
    static constexpr uint16_t F_INT      = 1 << 2;
    static constexpr uint16_t F_FLOAT    = 1 << 3;
    static constexpr uint16_t F_STR      = 1 << 4;
    static constexpr uint16_t F_RANGE    = 1 << 5;
    static constexpr uint16_t F_POINTER  = 1 << 6;
    static constexpr uint16_t F_ARRAY    = 1 << 7;
    static constexpr uint16_t F_FUNCTION = 1 << 8;
    static constexpr uint16_t F_TYPE     = 1 << 9;

    // attributes
    static constexpr uint16_t F_LITERAL   = 1 << 12;
    static constexpr uint16_t F_AGGREGATE = 1 << 13;
    static constexpr uint16_t F_HALF_OPEN = 1 << 14;

    enum ESign
    {
        eNotApplicable,
        eSigned,
        eUnsigned,
        eContextDependent,
    };

    static const TypeInfo* UnitType;
    static const TypeInfo* BoolType;
    static const TypeInfo* Int8Type;
    static const TypeInfo* Int16Type;
    static const TypeInfo* Int32Type;
    static const TypeInfo* Int64Type;
    static const TypeInfo* UInt8Type;
    static const TypeInfo* UInt16Type;
    static const TypeInfo* UInt32Type;
    static const TypeInfo* UInt64Type;
    static const TypeInfo* Float32Type;
    static const TypeInfo* Float64Type;
    static const TypeInfo* Float32LiteralType;
    static const TypeInfo* Float64LiteralType;
    static const TypeInfo* TypeType;

    static const TypeInfo* GetMinSignedIntTypeForSize(unsigned size);

    static const TypeInfo* GetMinUnsignedIntTypeForSize(unsigned size);

    static const TypeInfo* CreateFunctionType(
        TypeId id,
        unsigned numBits,
        ROString name,
        const std::vector<const TypeInfo*>& parameterTypes,
        const std::vector<ROString>& parameterNames,
        const TypeInfo* returnType);

    static const TypeInfo* CreateTypeAlias(ROString newName, const Token* newToken, const TypeInfo* typeInfo);

private:
    const Token* token;
    bool deleteData;

    struct TypeInfoData
    {
        TypeId id;
        unsigned numBits;
        uint16_t flags;
        ESign sign;
        std::unordered_map<ROString, const MemberInfo*> memberMap;
        std::vector<const MemberInfo*> members;
        const TypeInfo* innerType;
        std::vector<const TypeInfo*> paramTypes;
        std::vector<ROString> paramNames;
        const TypeInfo* returnType;
    };

    TypeInfoData* data;

    TypeInfo(
        ROString shortName,
        TypeInfoData* data,
        const Token* token
    );

public:
    ROString shortName;

    TypeInfo(
        TypeId id,
        unsigned numBits,
        uint16_t flags,
        ESign sign,
        ROString shortName,
        const TypeInfo* innerType = nullptr,
        const Token* token = nullptr
    );

    TypeInfo(const TypeInfo& other) = delete;

    virtual ~TypeInfo();

    TypeId GetId() const
    {
        return data->id;
    }

    bool IsSameAs(const TypeInfo& other) const
    {
        return data->id == other.data->id;
    }

    uint16_t GetFlags() const
    {
        return data->flags;
    }

    bool IsUnit() const
    {
        return (data->flags & F_UNIT) != 0;
    }

    bool IsBool() const
    {
        return (data->flags & F_BOOL) != 0;
    }

    bool IsInt() const
    {
        return (data->flags & F_INT) != 0;
    }

    bool IsFloat() const
    {
        return (data->flags & F_FLOAT) != 0;
    }

    bool IsNumeric() const
    {
        return (data->flags & (F_INT | F_FLOAT)) != 0;
    }

    bool IsStr() const
    {
        return (data->flags & F_STR) != 0;
    }

    bool IsRange() const
    {
        return (data->flags & F_RANGE) != 0;
    }

    bool IsPointer() const
    {
        return (data->flags & F_POINTER) != 0;
    }

    bool IsArray() const
    {
        return (data->flags & F_ARRAY) != 0;
    }

    bool IsFunction() const
    {
        return (data->flags & F_FUNCTION) != 0;
    }

    bool IsType() const
    {
        return (data->flags & F_TYPE) != 0;
    }

    ESign GetSign() const
    {
        return data->sign;
    }

    bool IsLiteral() const
    {
        return (data->flags & F_LITERAL) != 0;
    }

    bool IsAggregate() const
    {
        return (data->flags & F_AGGREGATE) != 0;
    }

    bool IsHalfOpen() const
    {
        return (data->flags & F_HALF_OPEN) != 0;
    }

    bool IsRecursiveStructDependency(ROString structName, std::vector<const Token*>& structTokenStack) const;

    bool IsOrContainsLiteral() const;

    virtual bool IsOrContainsNumericLiteral() const;

    virtual unsigned GetNumBits() const
    {
        return data->numBits;
    }

    // TODO: rename (GetFriendlyName?)
    ROString GetShortName() const
    {
        return shortName;
    }

    const MemberInfo* GetMember(ROString memberName) const;

    const std::vector<const MemberInfo*>& GetMembers() const
    {
        return data->members;
    }

    size_t GetMemberCount() const
    {
        return data->members.size();
    }

    bool AddMember(ROString name, const TypeInfo* type, bool isAssignable, const Token* token);

    const Token* GetToken() const
    {
        return token;
    }

    const TypeInfo* GetInnerType() const
    {
        return data->innerType;
    }

    const std::vector<const TypeInfo*>& GetParamTypes() const
    {
        return data->paramTypes;
    }

    const std::vector<ROString>& GetParamNames() const
    {
        return data->paramNames;
    }

    const TypeInfo* GetReturnType() const
    {
        return data->returnType;
    }
};

class NumericLiteralType : public TypeInfo
{
public:
    NumericLiteralType(
        TypeId id,
        ESign sign,
        unsigned signedNumBits,
        unsigned unsignedNumBits,
        ROString name
    );

    bool IsOrContainsNumericLiteral() const override;

    unsigned GetNumBits() const override;

    unsigned GetSignedNumBits() const;

    unsigned GetUnsignedNumBits() const;

private:
    unsigned signedNumBits;
    unsigned unsignedNumBits;
};

namespace std
{
    template<>
    struct hash<tuple<TypeInfo::ESign, unsigned, unsigned>>
    {
        std::size_t operator()(tuple<TypeInfo::ESign, unsigned, unsigned> t) const
        {
            std::size_t h = 257;
            h ^= static_cast<std::size_t>(get<0>(t));
            h *= 31;
            h ^= get<1>(t);
            h *= 31;
            h ^= get<2>(t);
            h *= 31;

            return h;
        }
    };

    template<>
    struct hash<tuple<TypeId, bool>>
    {
        std::size_t operator()(tuple<TypeId, bool> t) const
        {
            std::size_t h = static_cast<std::size_t>(get<0>(t));
            h <<= 1;
            h |= static_cast<std::size_t>(get<1>(t));

            return h;
        }
    };
}

#endif // TYPE_INFO_H_
