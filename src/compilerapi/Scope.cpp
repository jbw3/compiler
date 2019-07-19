#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Scope.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

Scope::Scope()
{
    Push();
}

Scope::~Scope()
{
    for (ScopeData* scope : scopes)
    {
        delete scope;
    }
}

void Scope::Push()
{
    scopes.push_back(new ScopeData);
}

void Scope::Pop()
{
    if (!scopes.empty())
    {
        delete scopes.back();
        scopes.pop_back();
    }

    // always make sure there is at least one scope on the stack
    if (scopes.empty())
    {
        Push();
    }
}

bool Scope::AddVariable(const std::string& name, SyntaxTree::VariableDefinition* variable)
{
    return AddVariable(name, variable, nullptr);
}

bool Scope::AddVariable(const string& name, VariableDefinition* variable, AllocaInst* value)
{
    auto rv = scopes.back()->variables.insert({name, {variable, value}});
    return rv.second;
}

VariableDefinition* Scope::GetVariable(const string& name) const
{
    VariableData* varData = GetVariableData(name);
    if (varData == nullptr)
    {
        return nullptr;
    }

    return varData->variable;
}

AllocaInst* Scope::GetValue(const string& name) const
{
    VariableData* varData = GetVariableData(name);
    if (varData == nullptr)
    {
        return nullptr;
    }

    return varData->value;
}

Scope::VariableData* Scope::GetVariableData(const string& name) const
{
    for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter)
    {
        unordered_map<string, VariableData>& variables = (*iter)->variables;
        auto varIter = variables.find(name);
        if (varIter != variables.cend())
        {
            return &varIter->second;
        }
    }

    return nullptr;
}
