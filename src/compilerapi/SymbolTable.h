#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <vector>

namespace llvm
{
class AllocaInst;
} // namespace llvm

class TypeInfo;

class SymbolTable
{
public:
    struct IdentifierData
    {
        const TypeInfo* type;
        llvm::AllocaInst* value;
        unsigned constValueIndex;

        bool IsConstant() const
        {
            return constValueIndex != NON_CONST_VALUE;
        }
    };

    SymbolTable();

    ~SymbolTable();

    void Push();

    void Pop();

    bool AddVariable(const std::string& name, const TypeInfo* type);

    bool AddVariable(const std::string& name, const TypeInfo* type, llvm::AllocaInst* value);

    bool AddConstant(const std::string& name, const TypeInfo* type, unsigned constValueIndex);

    IdentifierData* GetIdentifierData(const std::string& name) const;

private:
    static constexpr unsigned NON_CONST_VALUE = static_cast<unsigned>(-1);

    struct ScopeData
    {
        std::unordered_map<std::string, IdentifierData> identifiers;
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
