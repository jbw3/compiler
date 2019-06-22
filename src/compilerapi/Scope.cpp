#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Scope.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

bool Scope::AddVariable(const string& name, VariableDefinition* variable, AllocaInst* value)
{
    auto rv = variables.insert({name, {variable, value}});
    return rv.second;
}

VariableDefinition* Scope::GetVariable(const string& name) const
{
    auto iter = variables.find(name);
    if (iter == variables.cend())
    {
        return nullptr;
    }

    return iter->second.variable;
}

AllocaInst* Scope::GetValue(const string& name) const
{
    auto iter = variables.find(name);
    if (iter == variables.cend())
    {
        return nullptr;
    }

    return iter->second.value;
}

bool Scope::Contains(const string& name) const
{
    auto iter = variables.find(name);
    return iter != variables.cend();
}
