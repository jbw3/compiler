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

    unsigned NumBits;
    bool IsBool;
    bool IsInt;

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt
    );

private:
    static std::map<std::string, const TypeInfo*> types;
};

#endif // TYPE_INFO_H_
