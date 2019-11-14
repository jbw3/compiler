#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "LlvmIrOptimizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#pragma clang diagnostic pop

using namespace llvm;

LlvmOptimizer::LlvmOptimizer()
{
}

bool LlvmOptimizer::Optimize(Module* module)
{
    // ModulePassManager modulePassMgr(module);
    // modulePassMgr.addPass(createInstructionCombiningPass());
    // modulePassMgr.run(module);

    legacy::FunctionPassManager funPassMgr(module);

    funPassMgr.add(createPromoteMemoryToRegisterPass());
    funPassMgr.add(createInstructionCombiningPass());

    funPassMgr.doInitialization();
    for (Function& fun : module->functions())
    {
        funPassMgr.run(fun);
    }
    funPassMgr.doFinalization();

    return true;
}
