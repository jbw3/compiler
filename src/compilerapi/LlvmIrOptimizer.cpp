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
#include "llvm/Transforms/IPO.h"
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
        legacy::PassManager passMgr;

        passMgr.add(createPromoteMemoryToRegisterPass());
        passMgr.add(createInstructionCombiningPass());

        if (optimizationLevel >= 2)
        {
            passMgr.add(createTailCallEliminationPass());
            passMgr.add(createFunctionInliningPass());

            // loop optimization
            passMgr.add(createIndVarSimplifyPass());
            passMgr.add(createLoopRotatePass());
            passMgr.add(createLoopUnrollPass());
            passMgr.add(createLoopDeletionPass());

            passMgr.add(createPromoteMemoryToRegisterPass());
            passMgr.add(createInstructionCombiningPass());
            passMgr.add(createCFGSimplificationPass());
        }

        passMgr.run(*module);
    }

    return true;
}
