#ifndef SCOPE_H_
#define SCOPE_H_

#include <unordered_map>

namespace llvm
{
class Value;
} // namespace llvm

class Scope
{
public:
    bool AddVariable(llvm::Value* variable);

    llvm::Value* GetVariable(const std::string& name) const;

    bool Contains(const std::string& name) const;

private:
    std::unordered_map<std::string, llvm::Value*> variables;
};

#endif // SCOPE_H_
