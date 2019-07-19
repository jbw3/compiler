#ifndef SCOPE_H_
#define SCOPE_H_

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

class Scope
{
public:
    Scope();

    ~Scope();

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

#endif // SCOPE_H_
