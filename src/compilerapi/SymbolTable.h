#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <vector>

namespace llvm
{
class AllocaInst;
} // namespace llvm

namespace SyntaxTree
{
class VariableDefinition;
} // namespace SyntaxTree

class SymbolTable
{
public:
    SymbolTable();

    ~SymbolTable();

    void Push();

    void Pop();

    bool AddVariable(const std::string& name, SyntaxTree::VariableDefinition* variable);

    bool AddVariable(const std::string& name, SyntaxTree::VariableDefinition* variable, llvm::AllocaInst* value);

    SyntaxTree::VariableDefinition* GetVariable(const std::string& name) const;

    llvm::AllocaInst* GetValue(const std::string& name) const;

private:
    struct VariableData
    {
        SyntaxTree::VariableDefinition* variable;
        llvm::AllocaInst* value;
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
