#ifndef TYPE_REGISTRY_H_
#define TYPE_REGISTRY_H_

#include "ROString.h"
#include <unordered_map>
#include <vector>

namespace llvm
{
class TargetMachine;
}
class TypeInfo;

class TypeRegistry
{
public:
    TypeRegistry(const llvm::TargetMachine* targetMachine);

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

    const TypeInfo* GetType(const std::string& typeName);

private:
    unsigned pointerSize;
    TypeInfo* intSizeType;
    TypeInfo* uintSizeType;
    TypeInfo* stringType;
    std::unordered_map<std::string, const TypeInfo*> types;
};

#endif // TYPE_REGISTRY_H_
