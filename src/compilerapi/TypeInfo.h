#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>

namespace llvm
{
class TargetMachine;
}
class TypeInfo;
class UnitTypeInfo;

class MemberInfo
{
public:
    MemberInfo(const std::string& name, unsigned index, const TypeInfo* type, bool isAssignable);

    const std::string& GetName() const;

    unsigned GetIndex() const;

    const TypeInfo* GetType() const;

    bool GetIsAssignable() const;

private:
    std::string name;
    unsigned index;
    const TypeInfo* type;
    bool isAssignable;
};

class TypeInfo
{
public:
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

    static void InitTypes(const llvm::TargetMachine* targetMachine);

    static const TypeInfo* GetIntSizeType();

    static const TypeInfo* GetUIntSizeType();

    static const TypeInfo* GetMinSignedIntTypeForSize(unsigned size);

    static const TypeInfo* GetStringPointerType();

    static const TypeInfo* GetType(const std::string& typeName);

    static bool RegisterType(const TypeInfo* typeInfo);

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt,
        ESign sign,
        bool isAggregate,
        const std::string& shortName
    );

    virtual ~TypeInfo();

    virtual bool IsSameAs(const TypeInfo& other) const = 0;

    bool IsBool() const;

    bool IsInt() const;

    ESign GetSign() const;

    bool IsAggregate() const;

    virtual unsigned GetNumBits() const;

    const std::string& GetShortName() const;

    const MemberInfo* GetMember(const std::string& memberName) const;

    const std::map<std::string, const MemberInfo*>& GetMembers() const;

    size_t GetMemberCount() const;

    bool AddMember(const std::string& name, const TypeInfo* type, bool isAssignable);

private:
    static TypeInfo* intSizeType;
    static TypeInfo* uintSizeType;
    static TypeInfo* stringPointerType;
    static std::map<std::string, const TypeInfo*> types;

    unsigned numBits;
    bool isBool;
    bool isInt;
    ESign sign;
    bool isAggregate;
    std::string shortName;
    std::map<std::string, const MemberInfo*> members;
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
        bool isBool,
        bool isInt,
        ESign sign,
        const std::string& shortName
    );

    bool IsSameAs(const TypeInfo& other) const override;
};

class ContextInt : public TypeInfo
{
public:
    ContextInt(unsigned signedNumBits, unsigned unsignedNumBits);

    bool IsSameAs(const TypeInfo& other) const override;

    unsigned GetNumBits() const override;

    unsigned GetSignedNumBits() const;

    unsigned GetUnsignedNumBits() const;

private:
    unsigned signedNumBits;
    unsigned unsignedNumBits;
};

class StringPointerType : public TypeInfo
{
public:
    StringPointerType(unsigned numBits);

    bool IsSameAs(const TypeInfo& other) const override;
};

class AggregateType : public TypeInfo
{
public:
    AggregateType(const std::string& name);

    bool IsSameAs(const TypeInfo& other) const override;
};

#endif // TYPE_INFO_H_
