#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "Config.h"
#include "LlvmIrOptimizer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

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
