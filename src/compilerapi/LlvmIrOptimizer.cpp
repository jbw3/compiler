#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "Config.h"
#include "LlvmIrOptimizer.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
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
    // create the analysis managers (they must be declared in this order,
    // see https://llvm.org/docs/NewPassManager.html)
    LoopAnalysisManager loopAnalysisMgr;
    FunctionAnalysisManager functionAnalysisMgr;
    CGSCCAnalysisManager cgsccAnalysisMgr;
    ModuleAnalysisManager moduleAnalysisMgr;

    PassBuilder passBuilder;

    passBuilder.registerModuleAnalyses(moduleAnalysisMgr);
    passBuilder.registerCGSCCAnalyses(cgsccAnalysisMgr);
    passBuilder.registerFunctionAnalyses(functionAnalysisMgr);
    passBuilder.registerLoopAnalyses(loopAnalysisMgr);
    passBuilder.crossRegisterProxies(loopAnalysisMgr, functionAnalysisMgr, cgsccAnalysisMgr, moduleAnalysisMgr);

    OptimizationLevel llvmOptimizationLevel = OptimizationLevel::O0;
    switch (optimizationLevel)
    {
    case 0:
    default:
        llvmOptimizationLevel = OptimizationLevel::O0;
        break;
    case 1:
        llvmOptimizationLevel = OptimizationLevel::O1;
        break;
    case 2:
        llvmOptimizationLevel = OptimizationLevel::O2;
        break;
    }

    ModulePassManager modulePassMgr = passBuilder.buildPerModuleDefaultPipeline(llvmOptimizationLevel);
    modulePassMgr.run(*module, moduleAnalysisMgr);

    return true;
}
