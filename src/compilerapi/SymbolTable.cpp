#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "SymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

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
    IdentifierData* data = GetIdentifierData(name);

    // only insert if there is not already a variable with this name
    if (data == nullptr)
    {
        scopes.back()->identifiers.insert({name, {type, value, NON_CONST_VALUE}});
        return true;
    }
    else
    {
        return false;
    }
}

bool SymbolTable::AddConstant(const string& name, const TypeInfo* type, unsigned constValueIndex)
{
    IdentifierData* data = GetIdentifierData(name);

    // only insert if there is not already an identifier with this name
    if (data == nullptr)
    {
        scopes.back()->identifiers.insert({name, {type, nullptr, constValueIndex}});
        return true;
    }
    else
    {
        return false;
    }
}

SymbolTable::IdentifierData* SymbolTable::GetIdentifierData(const string& name) const
{
    for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter)
    {
        unordered_map<string, IdentifierData>& identifiers = (*iter)->identifiers;
        auto identifierIter = identifiers.find(name);
        if (identifierIter != identifiers.cend())
        {
            return &identifierIter->second;
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
