#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Scope.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

bool Scope::AddVariable(Value* variable)
{
    auto rv = variables.insert({variable->getName(), variable});
    return rv.second;
}

Value* Scope::GetVariable(const string& name) const
{
    auto iter = variables.find(name);
    if (iter == variables.cend())
    {
        return nullptr;
    }

    return iter->second;
}

bool Scope::Contains(const string& name) const
{
    auto iter = variables.find(name);
    return iter != variables.cend();
}
