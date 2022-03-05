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
class RangeType;
class Token;
class TypeInfo;
class UnitTypeInfo;

const char* const POINTER_TYPE_TOKEN = "&";
const char* const DOUBLE_POINTER_TYPE_TOKEN = "&&";
const char* const ARRAY_TYPE_START_TOKEN = "[";
const char* const ARRAY_TYPE_END_TOKEN = "]";

class MemberInfo
{
public:
    MemberInfo(const std::string& name, unsigned index, const TypeInfo* type, bool isStorage, const Token* token);

    const std::string& GetName() const;

    unsigned GetIndex() const;

    const TypeInfo* GetType() const;

    bool GetIsStorage() const;

    const Token* GetToken() const;

private:
    std::string name;
    unsigned index;
    const TypeInfo* type;
    const Token* token;
    bool isStorage;
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
    static constexpr uint16_t F_AGGREGATE = 1 << 12;
    static constexpr uint16_t F_HALF_OPEN = 1 << 13;

    enum ESign
    {
        eNotApplicable,
        eSigned,
        eUnsigned,
        eContextDependent,
    };

    static const UnitTypeInfo* UnitType;
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
    static const TypeInfo* TypeType;

    static const TypeInfo* GetMinSignedIntTypeForSize(unsigned size);

    static const TypeInfo* GetMinUnsignedIntTypeForSize(unsigned size);

    static const TypeInfo* CreateFunctionType(
        unsigned numBits,
        const std::string& uniqueName,
        const std::string& name,
        const std::vector<const TypeInfo*>& parameterTypes,
        const std::vector<std::string>& parameterNames,
        const TypeInfo* returnType);

    TypeInfo(
        unsigned numBits,
        uint16_t flags,
        ESign sign,
        const std::string& uniqueName,
        const std::string& shortName,
        const TypeInfo* innerType = nullptr
    );

    virtual ~TypeInfo();

    virtual bool IsSameAs(const TypeInfo& other) const = 0;

    uint16_t GetFlags() const;

    bool IsUnit() const;

    bool IsBool() const;

    bool IsInt() const;

    bool IsFloat() const;

    bool IsNumeric() const;

    bool IsStr() const;

    bool IsRange() const;

    bool IsPointer() const;

    bool IsArray() const;

    bool IsFunction() const;

    bool IsType() const;

    ESign GetSign() const;

    bool IsAggregate() const;

    bool IsHalfOpen() const;

    virtual bool IsNumericLiteral() const;

    virtual unsigned GetNumBits() const;

    const std::string& GetUniqueName() const;

    // TODO: rename (GetFriendlyName?)
    const std::string& GetShortName() const;

    const MemberInfo* GetMember(const std::string& memberName) const;

    const std::vector<const MemberInfo*>& GetMembers() const;

    size_t GetMemberCount() const;

    bool AddMember(const std::string& name, const TypeInfo* type, bool isAssignable, const Token* token);

    const TypeInfo* GetInnerType() const;

    const std::vector<const TypeInfo*>& GetParamTypes() const;

    const std::vector<std::string>& GetParamNames() const;

    const TypeInfo* GetReturnType() const;

private:
    unsigned numBits;
    uint16_t flags;
    ESign sign;
    std::string uniqueName;
    std::string shortName;
    std::map<std::string, const MemberInfo*> memberMap;
    std::vector<const MemberInfo*> members;
    const TypeInfo* innerType;
    std::vector<const TypeInfo*> paramTypes;
    std::vector<std::string> paramNames;
    const TypeInfo* returnType;
};

class UnitTypeInfo : public TypeInfo
{
public:
    UnitTypeInfo();

    bool IsSameAs(const TypeInfo& other) const override;
};

class PrimitiveType : public TypeInfo
{
public:
    PrimitiveType(
        unsigned numBits,
        uint16_t flags,
        ESign sign,
        const std::string& uniqueName,
        const std::string& shortName,
        const TypeInfo* innerType = nullptr
    );

    bool IsSameAs(const TypeInfo& other) const override;
};

class NumericLiteralType : public TypeInfo
{
public:
    static const NumericLiteralType* Create(unsigned signedNumBits, unsigned unsignedNumBits);

    static const NumericLiteralType* CreateSigned(unsigned numBits);

    static const NumericLiteralType* CreateUnsigned(unsigned numBits);

    bool IsSameAs(const TypeInfo& other) const override;

    bool IsNumericLiteral() const override;

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

class StringType : public TypeInfo
{
public:
    StringType(unsigned numBits, const TypeInfo* sizeType, const TypeInfo* pointerType);

    bool IsSameAs(const TypeInfo& other) const override;
};

class AggregateType : public TypeInfo
{
public:
    AggregateType(const std::string& name, const Token* token);

    bool IsSameAs(const TypeInfo& other) const override;

    const Token* GetToken() const;

private:
    const Token* token;
};

#endif // TYPE_INFO_H_
