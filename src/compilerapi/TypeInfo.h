#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_

#include <map>

class TypeInfo
{
public:
    unsigned numBits;
    bool isBool;
    bool isInt;

    static const TypeInfo* GetType(const std::string& typeName);

    TypeInfo(
        unsigned numBits,
        bool isBool,
        bool isInt
    );

private:
    static std::map<std::string, const TypeInfo*> types;
};

#endif // TYPE_INFO_H_
