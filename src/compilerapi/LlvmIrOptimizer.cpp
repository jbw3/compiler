#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Config.h"
#include "LlvmIrOptimizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#pragma clang diagnostic pop

using namespace llvm;

LlvmOptimizer::LlvmOptimizer(const Config& config) :
    optimizationLevel(config.optimizationLevel)
{
}

bool LlvmOptimizer::Optimize(Module* module)
{
    if (optimizationLevel >= 1)
    {
        legacy::FunctionPassManager funPassMgr(module);

        funPassMgr.add(createPromoteMemoryToRegisterPass());
        funPassMgr.add(createInstructionCombiningPass());

        if (optimizationLevel >= 2)
        {
            funPassMgr.add(createTailCallEliminationPass());

            // loop optimization
            funPassMgr.add(createIndVarSimplifyPass());
            funPassMgr.add(createLoopRotatePass());
            funPassMgr.add(createLoopUnrollPass());
            funPassMgr.add(createLoopDeletionPass());

            funPassMgr.add(createCFGSimplificationPass());
            funPassMgr.add(createInstructionCombiningPass());
        }

        funPassMgr.doInitialization();
        for (Function& fun : module->functions())
        {
            funPassMgr.run(fun);
        }
        funPassMgr.doFinalization();
    }

    return true;
}
