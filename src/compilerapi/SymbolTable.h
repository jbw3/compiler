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
    SymbolTable();

    ~SymbolTable();

    void Push();

    void Pop();

    bool AddVariable(const std::string& name, const TypeInfo* type);

    bool AddVariable(const std::string& name, const TypeInfo* type, llvm::AllocaInst* value);

    bool AddConstant(const std::string& name, const TypeInfo* type);

    const TypeInfo* GetVariableType(const std::string& name) const;

    llvm::AllocaInst* GetValue(const std::string& name) const;

    bool IsIdentifierConstant(const std::string& name) const;

private:
    struct VariableData
    {
        const TypeInfo* type;
        llvm::AllocaInst* value;
        bool isConstant;
    };

    struct ScopeData
    {
        std::unordered_map<std::string, VariableData> variables;
    };

    std::vector<ScopeData*> scopes;

    VariableData* GetVariableData(const std::string& name) const;
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
