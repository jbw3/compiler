#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>
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
    MemberInfo(const std::string& name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token);

    const std::string& GetName() const
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
    std::string name;
    unsigned index;
    bool isStorage;
    const TypeInfo* type;
    const Token* token;
};

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

    static TypeInfo* CreateAggregateType(const std::string& name, const Token* token);

    static const TypeInfo* CreateFunctionType(
        unsigned numBits,
        const std::string& uniqueName,
        const std::string& name,
        const std::vector<const TypeInfo*>& parameterTypes,
        const std::vector<ROString>& parameterNames,
        const TypeInfo* returnType);

    TypeInfo(
        unsigned numBits,
        uint16_t flags,
        ESign sign,
        const std::string& uniqueName,
        const std::string& shortName,
        const TypeInfo* innerType = nullptr,
        const Token* token = nullptr
    );

    virtual ~TypeInfo();

    virtual bool IsSameAs(const TypeInfo& other) const;

    uint16_t GetFlags() const
    {
        return flags;
    }

    bool IsUnit() const
    {
        return (flags & F_UNIT) != 0;
    }

    bool IsBool() const
    {
        return (flags & F_BOOL) != 0;
    }

    bool IsInt() const
    {
        return (flags & F_INT) != 0;
    }

    bool IsFloat() const
    {
        return (flags & F_FLOAT) != 0;
    }

    bool IsNumeric() const
    {
        return (flags & (F_INT | F_FLOAT)) != 0;
    }

    bool IsStr() const
    {
        return (flags & F_STR) != 0;
    }

    bool IsRange() const
    {
        return (flags & F_RANGE) != 0;
    }

    bool IsPointer() const
    {
        return (flags & F_POINTER) != 0;
    }

    bool IsArray() const
    {
        return (flags & F_ARRAY) != 0;
    }

    bool IsFunction() const
    {
        return (flags & F_FUNCTION) != 0;
    }

    bool IsType() const
    {
        return (flags & F_TYPE) != 0;
    }

    ESign GetSign() const
    {
        return sign;
    }

    bool IsLiteral() const
    {
        return (flags & F_LITERAL) != 0;
    }

    bool IsAggregate() const
    {
        return (flags & F_AGGREGATE) != 0;
    }

    bool IsHalfOpen() const
    {
        return (flags & F_HALF_OPEN) != 0;
    }

    bool IsOrContainsLiteral() const;

    virtual bool IsOrContainsNumericLiteral() const;

    virtual unsigned GetNumBits() const
    {
        return numBits;
    }

    const std::string& GetUniqueName() const
    {
        return uniqueName;
    }

    // TODO: rename (GetFriendlyName?)
    const std::string& GetShortName() const
    {
        return shortName;
    }

    const MemberInfo* GetMember(const std::string& memberName) const;

    const std::vector<const MemberInfo*>& GetMembers() const
    {
        return members;
    }

    size_t GetMemberCount() const
    {
        return members.size();
    }

    bool AddMember(const std::string& name, const TypeInfo* type, bool isAssignable, const Token* token);

    const Token* GetToken() const
    {
        return token;
    }

    const TypeInfo* GetInnerType() const
    {
        return innerType;
    }

    const std::vector<const TypeInfo*>& GetParamTypes() const
    {
        return paramTypes;
    }

    const std::vector<ROString>& GetParamNames() const
    {
        return paramNames;
    }

    const TypeInfo* GetReturnType() const
    {
        return returnType;
    }

private:
    unsigned numBits;
    uint16_t flags;
    ESign sign;
    std::string uniqueName;
    std::string shortName;
    std::map<std::string, const MemberInfo*> memberMap;
    std::vector<const MemberInfo*> members;
    const Token* token;
    const TypeInfo* innerType;
    std::vector<const TypeInfo*> paramTypes;
    std::vector<ROString> paramNames;
    const TypeInfo* returnType;
};

class NumericLiteralType : public TypeInfo
{
public:
    static const NumericLiteralType* Create(unsigned signedNumBits, unsigned unsignedNumBits);

    static const NumericLiteralType* CreateSigned(unsigned numBits);

    static const NumericLiteralType* CreateUnsigned(unsigned numBits);

    bool IsSameAs(const TypeInfo& other) const override;

    bool IsOrContainsNumericLiteral() const override;

    unsigned GetNumBits() const override;

    unsigned GetSignedNumBits() const;

    unsigned GetUnsignedNumBits() const;

    const TypeInfo* GetMinSizeType(ESign sign) const;

private:
    static std::unordered_map
    <
        std::tuple<ESign, unsigned, unsigned>,
        const NumericLiteralType*
    > instances;

    unsigned signedNumBits;
    unsigned unsignedNumBits;

    static const NumericLiteralType* Create(ESign sign, unsigned signedNumBits, unsigned unsignedNumBits, const char* name);

    NumericLiteralType(
        ESign sign,
        unsigned signedNumBits,
        unsigned unsignedNumBits,
        const std::string& name
    );
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
}

#endif // TYPE_INFO_H_
