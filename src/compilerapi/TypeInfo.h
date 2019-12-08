#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>

namespace llvm
{
class TargetMachine;
}
class UnitTypeInfo;

class TypeInfo
{
public:
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

    static const TypeInfo* GetStringPointerType();

    static const TypeInfo* GetType(const std::string& typeName);

    static bool RegisterType(const std::string& typeName, const TypeInfo* typeInfo);

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt,
        bool isSigned
    );

    virtual bool IsSameAs(const TypeInfo& other) const = 0;

    bool IsBool() const;

    bool IsInt() const;

    bool IsSigned() const;

    unsigned GetNumBits() const;

private:
    static TypeInfo* stringPointerType;
    static std::map<std::string, const TypeInfo*> types;

    unsigned numBits;
    bool isBool;
    bool isInt;
    bool isSigned;
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
        bool isSigned
    );

    bool IsSameAs(const TypeInfo& other) const override;
};

class StringPointerType : public TypeInfo
{
public:
    StringPointerType(unsigned numBits);

    bool IsSameAs(const TypeInfo& other) const override;
};

#endif // TYPE_INFO_H_
