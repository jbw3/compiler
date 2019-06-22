#ifndef SCOPE_H_
#define SCOPE_H_

#include <unordered_map>

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
    bool AddVariable(const std::string& name, SyntaxTree::VariableDefinition* variable, llvm::AllocaInst* value);

    SyntaxTree::VariableDefinition* GetVariable(const std::string& name) const;

    llvm::AllocaInst* GetValue(const std::string& name) const;

    bool Contains(const std::string& name) const;

private:
    struct Data
    {
        SyntaxTree::VariableDefinition* variable;
        llvm::AllocaInst* value;
    };

    std::unordered_map<std::string, Data> variables;
};

#endif // SCOPE_H_
