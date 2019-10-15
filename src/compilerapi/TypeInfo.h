#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>

class TypeInfo
{
public:
    static const TypeInfo* BoolType;
    static const TypeInfo* Int8Type;
    static const TypeInfo* Int16Type;
    static const TypeInfo* Int32Type;
    static const TypeInfo* Int64Type;

    static const TypeInfo* GetType(const std::string& typeName);

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt
    );

    virtual bool IsSameAs(const TypeInfo& other) const = 0;

    bool IsBool() const;

    bool IsInt() const;

    unsigned GetNumBits() const;

private:
    static std::map<std::string, const TypeInfo*> types;

    unsigned numBits;
    bool isBool;
    bool isInt;
};

class PrimitiveType : public TypeInfo
{
public:
    PrimitiveType(
        unsigned numBits,
        bool isBool,
        bool isInt
    );

    bool IsSameAs(const TypeInfo& other) const override;
};

#endif // TYPE_INFO_H_
