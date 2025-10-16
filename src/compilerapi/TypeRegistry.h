#ifndef TYPE_REGISTRY_H_
#define TYPE_REGISTRY_H_

#include "ROString.h"
#include "TypeInfo.h"
#include <unordered_map>
#include <vector>

namespace SyntaxTree
{
class FunctionDeclaration;
}
class CompilerContext;

class FunctionTypeKey
{
public:
    FunctionTypeKey(const SyntaxTree::FunctionDeclaration* functionDeclaration);

    FunctionTypeKey(const std::vector<const TypeInfo*>& parameterTypes, const TypeInfo* returnType);

    ~FunctionTypeKey() = default;

    bool operator ==(const FunctionTypeKey& other) const;

    std::vector<TypeId> paramTypeIds;
    TypeId returnTypeId;
};

namespace std
{
    template<>
    struct hash<FunctionTypeKey>
    {
        std::size_t operator()(const FunctionTypeKey& key) const
        {
            std::size_t h = 257;
            h ^= static_cast<std::size_t>(key.returnTypeId);
            h *= 31;
            for (TypeId paramTypeId : key.paramTypeIds)
            {
                h ^= static_cast<std::size_t>(paramTypeId);
                h *= 31;
            }

            return h;
        }
    };
}

class TypeRegistry
{
public:
    TypeRegistry(CompilerContext& compilerContext);

    ~TypeRegistry();

    unsigned GetPointerSize() const;

    const TypeInfo* GetIntSizeType() const;

    const TypeInfo* GetUIntSizeType() const;

    const TypeInfo* GetStringType() const;

    const TypeInfo* GetRangeType(const TypeInfo* memberType, bool isHalfOpen);

    TypeInfo* GetAggregateType(ROString name, const Token* token);

    const TypeInfo* GetFunctionType(const SyntaxTree::FunctionDeclaration* functionDeclaration);

    const TypeInfo* GetFunctionType(
        const std::vector<const TypeInfo*>& parameterTypes,
        const std::vector<ROString>& parameterNames,
        const TypeInfo* returnType
    );

    const TypeInfo* GetPointerToType(const TypeInfo* type);

    const TypeInfo* GetArrayOfType(const TypeInfo* type);

    const TypeInfo* GetTypeAlias(ROString newName, const Token* newToken, const TypeInfo* typeInfo);

    const NumericLiteralType* CreateNumericLiteralType(unsigned signedNumBits, unsigned unsignedNumBits);

    const NumericLiteralType* CreateSignedNumericLiteralType(unsigned numBits);

    const NumericLiteralType* CreateUnsignedNumericLiteralType(unsigned numBits);

    const TypeInfo* GetMinSizeNumericLiteralType(const NumericLiteralType* numLitType, TypeInfo::ESign sign);

private:
    TypeId GetNextTypeId();

    CompilerContext& compilerContext;
    TypeInfo* intSizeType;
    TypeInfo* uintSizeType;
    TypeInfo* stringType;
    size_t typeDeleteStartIndex;
    std::vector<const TypeInfo*> types;
    std::unordered_map<TypeId, const TypeInfo*> pointerTypes; // maps type IDs to a pointer to that type
    std::unordered_map<TypeId, const TypeInfo*> arrayTypes; // maps type IDs to an array of that type
    std::unordered_map<std::tuple<TypeId, bool>, const TypeInfo*> rangeTypes; // maps (type ID, half-open/closed) to a range of that type
    std::unordered_map<FunctionTypeKey, const TypeInfo*> functionTypes; // maps function return type/param types to a function type
    unsigned pointerSize;
    std::unordered_map
    <
        std::tuple<TypeInfo::ESign, unsigned, unsigned>,
        const NumericLiteralType*
    > numericLiteralTypes;

    ROString GetNumericLiteralTypeUniqueName(unsigned signedNumBits, unsigned unsignedNumBits);

    const NumericLiteralType* CreateNumericLiteralType(TypeInfo::ESign sign, unsigned signedNumBits, unsigned unsignedNumBits, ROString name);
};

#endif // TYPE_REGISTRY_H_
