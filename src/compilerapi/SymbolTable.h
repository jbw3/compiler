#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include "ROString.h"
#include <unordered_map>
#include <vector>

namespace llvm
{
class AllocaInst;
} // namespace llvm

class CompilerContext;
class Token;
class TypeInfo;

class SymbolTable
{
public:
    struct IdentifierData
    {
        const TypeInfo* type;
        const Token* token;
        llvm::AllocaInst* value;
        unsigned constValueIndex;

        bool IsConstant() const
        {
            return constValueIndex != NON_CONST_VALUE;
        }
    };

    SymbolTable(CompilerContext& compilerContext);

    ~SymbolTable();

    void Push();

    void Pop();

    bool AddVariable(ROString name, const Token* token, const TypeInfo* type);

    bool AddVariable(ROString name, const Token* token, const TypeInfo* type, llvm::AllocaInst* value);

    bool AddConstant(ROString name, const Token* token, const TypeInfo* type, unsigned constValueIndex);

    IdentifierData* GetIdentifierData(ROString name) const;

    bool IsAtGlobalScope() const
    {
        return scopes.size() == 1;
    }

private:
    static constexpr unsigned NON_CONST_VALUE = static_cast<unsigned>(-1);

    struct ScopeData
    {
        std::unordered_map<ROString, IdentifierData> identifiers;
    };

    std::vector<ScopeData*> scopes;
};

class Scope
{
public:
    Scope(SymbolTable& symbolTable);

    ~Scope();

private:
    SymbolTable& symbolTable;
};

#endif // SYMBOL_TABLE_H_
