#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>

class TypeInfo
{
public:
    static const TypeInfo* boolType;
    static const TypeInfo* int32Type;

    static const TypeInfo* GetType(const std::string& typeName);

    unsigned numBits;
    bool isBool;
    bool isInt;

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt
    );

private:
    static std::map<std::string, const TypeInfo*> types;
};

#endif // TYPE_INFO_H_
