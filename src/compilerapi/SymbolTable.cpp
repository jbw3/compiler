#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "CompilerContext.h"
#include "SymbolTable.h"
#include "TypeInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instructions.h"
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

using namespace llvm;
using namespace std;

SymbolTable::SymbolTable(CompilerContext& compilerContext)
{
    Push();

    for (unsigned id = 0; id < compilerContext.GetBasicTypeCount(); ++id)
    {
        const TypeInfo* type = compilerContext.GetTypeConstantValue(id);
        AddConstant(type->GetShortName(), nullptr, TypeInfo::TypeType, id);
    }
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

bool SymbolTable::AddVariable(ROString name, const Token* token, const TypeInfo* type)
{
    return AddVariable(name, token, type, nullptr);
}

bool SymbolTable::AddVariable(ROString name, const Token* token, const TypeInfo* type, AllocaInst* value)
{
    IdentifierData* data = GetIdentifierData(name);

    // only insert if there is not already a variable with this name
    if (data == nullptr)
    {
        scopes.back()->identifiers.insert({name, {type, token, value, NON_CONST_VALUE}});
        return true;
    }
    else
    {
        return false;
    }
}

bool SymbolTable::AddConstant(ROString name, const Token* token, const TypeInfo* type, unsigned constValueIndex)
{
    IdentifierData* data = GetIdentifierData(name);

    // only insert if there is not already an identifier with this name
    if (data == nullptr)
    {
        scopes.back()->identifiers.insert({name, {type, token, nullptr, constValueIndex}});
        return true;
    }
    else
    {
        return false;
    }
}

SymbolTable::IdentifierData* SymbolTable::GetIdentifierData(ROString name) const
{
    for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter)
    {
        unordered_map<ROString, IdentifierData>& identifiers = (*iter)->identifiers;
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
