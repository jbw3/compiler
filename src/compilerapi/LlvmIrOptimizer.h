#ifndef LLVM_OPTIMIZER_H_
#define LLVM_OPTIMIZER_H_

namespace llvm
{
class Module;
}
class Config;

class LlvmOptimizer
{
public:
    LlvmOptimizer(const Config& config);

    bool Optimize(llvm::Module* module);

private:
    unsigned optimizationLevel;
};

#endif // LLVM_OPTIMIZER_H_
