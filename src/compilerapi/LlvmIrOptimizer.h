#ifndef LLVM_OPTIMIZER_H_
#define LLVM_OPTIMIZER_H_

namespace llvm
{
class Module;
}

class LlvmOptimizer
{
public:
    LlvmOptimizer();

    bool Optimize(llvm::Module* module);
};

#endif // LLVM_OPTIMIZER_H_
