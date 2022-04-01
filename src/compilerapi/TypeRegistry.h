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

class TypeRegistry
{
public:
    TypeRegistry(CompilerContext& compilerContext);

    unsigned GetPointerSize() const;

    const TypeInfo* GetIntSizeType() const;

    const TypeInfo* GetUIntSizeType() const;

    const TypeInfo* GetStringType() const;

    const TypeInfo* GetRangeType(const TypeInfo* memberType, bool isHalfOpen);

    const TypeInfo* GetFunctionType(const SyntaxTree::FunctionDeclaration* functionDeclaration);

    const TypeInfo* GetFunctionType(
        const std::vector<const TypeInfo*>& parameterTypes,
        const std::vector<ROString>& parameterNames,
        const TypeInfo* returnType
    );

    const TypeInfo* GetPointerToType(const TypeInfo* type);

    const TypeInfo* GetArrayOfType(const TypeInfo* type);

    bool RegisterType(const TypeInfo* typeInfo);

    const TypeInfo* GetType(ROString typeName);

    const NumericLiteralType* CreateNumericLiteralType(unsigned signedNumBits, unsigned unsignedNumBits);

    const NumericLiteralType* CreateSignedNumericLiteralType(unsigned numBits);

    const NumericLiteralType* CreateUnsignedNumericLiteralType(unsigned numBits);

    const TypeInfo* GetMinSizeNumericLiteralType(const NumericLiteralType* numLitType, TypeInfo::ESign sign);

private:
    CompilerContext& compilerContext;
    TypeInfo* intSizeType;
    TypeInfo* uintSizeType;
    TypeInfo* stringType;
    std::unordered_map<ROString, const TypeInfo*> types;
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
