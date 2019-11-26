#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "SymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

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

bool SymbolTable::AddVariable(const std::string& name, SyntaxTree::VariableDeclaration* variable)
{
    return AddVariable(name, variable, nullptr);
}

bool SymbolTable::AddVariable(const string& name, VariableDeclaration* variable, AllocaInst* value)
{
    auto rv = scopes.back()->variables.insert({name, {variable, value}});
    return rv.second;
}

VariableDeclaration* SymbolTable::GetVariable(const string& name) const
{
    VariableData* varData = GetVariableData(name);
    if (varData == nullptr)
    {
        return nullptr;
    }

    return varData->variable;
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
