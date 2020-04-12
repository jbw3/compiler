#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "SymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

SymbolTable::SymbolTable()
{
    Push();
}

SymbolTable::~SymbolTable()
{
    for (ScopeData* scope : scopes)
    {
        delete scope;
    }
}

void SymbolTable::Push()
{
    scopes.push_back(new ScopeData);
}

void SymbolTable::Pop()
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

bool SymbolTable::AddVariable(const std::string& name, const TypeInfo* type)
{
    return AddVariable(name, type, nullptr);
}

bool SymbolTable::AddVariable(const string& name, const TypeInfo* type, AllocaInst* value)
{
    VariableData* varData = GetVariableData(name);

    // only insert if there is not already a variable with this name
    if (varData == nullptr)
    {
        scopes.back()->variables.insert({name, {type, value}});
        return true;
    }
    else
    {
        return false;
    }
}

const TypeInfo* SymbolTable::GetVariableType(const string& name) const
{
    VariableData* varData = GetVariableData(name);
    if (varData == nullptr)
    {
        return nullptr;
    }

    return varData->type;
}

AllocaInst* SymbolTable::GetValue(const string& name) const
{
    VariableData* varData = GetVariableData(name);
    if (varData == nullptr)
    {
        return nullptr;
    }

    return varData->value;
}

SymbolTable::VariableData* SymbolTable::GetVariableData(const string& name) const
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

Scope::Scope(SymbolTable& symbolTable) :
    symbolTable(symbolTable)
{
    symbolTable.Push();
}

Scope::~Scope()
{
    symbolTable.Pop();
}
