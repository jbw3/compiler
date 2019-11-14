#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "AssemblyGenerator.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

AssemblyGenerator::AssemblyGenerator(const Config& config) :
    architecture(config.architecture),
    assemblyType(config.assemblyType)
{
    if (config.outFilename.empty())
    {
        size_t idx = config.inFilename.rfind('.');
        string baseName = config.inFilename.substr(0, idx);

        switch (config.assemblyType)
        {
            case Config::eLlvmIr:
                outFilename = baseName + ".ll";
                break;
            case Config::eMachineText:
                outFilename = baseName + ".s";
                break;
            case Config::eMachineBinary:
                outFilename = baseName + ".o";
                break;
        }
    }
    else
    {
        outFilename = config.outFilename;
    }
}

bool AssemblyGenerator::Generate(Module* module)
{
    if (assemblyType == Config::eLlvmIr)
    {
        error_code ec;
        raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
        if (ec)
        {
            cerr << ec.message();
            return false;
        }

        // print LLVM IR
        module->print(outFile, nullptr);
    }
    else if (assemblyType == Config::eMachineText || assemblyType == Config::eMachineBinary)
    {
        // default target triple to the current machine
        Triple targetTripple(sys::getDefaultTargetTriple());

        // override the architecture if configured
        if (!architecture.empty())
        {
            Triple::ArchType archType = Triple::getArchTypeForLLVMName(architecture);
            targetTripple.setArch(archType);
        }

        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        string errorMsg;
        const Target* target = TargetRegistry::lookupTarget(targetTripple.str(), errorMsg);
        if (target == nullptr)
        {
            cerr << errorMsg;
            return false;
        }

        TargetOptions options;
        auto relocModel = Optional<Reloc::Model>();
        TargetMachine* targetMachine =
            target->createTargetMachine(targetTripple.str(), "generic", "", options, relocModel);

        module->setDataLayout(targetMachine->createDataLayout());
        module->setTargetTriple(targetTripple.str());

        error_code ec;
        raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
        if (ec)
        {
            cerr << ec.message();
            return false;
        }

        legacy::PassManager passManager;
        TargetMachine::CodeGenFileType fileType = (assemblyType == Config::eMachineBinary)
                                                      ? TargetMachine::CGFT_ObjectFile
                                                      : TargetMachine::CGFT_AssemblyFile;
        if (targetMachine->addPassesToEmitFile(passManager, outFile, nullptr, fileType))
        {
            cerr << "Target machine cannot emit a file of this type\n";
            return false;
        }

        passManager.run(*module);
    }
    else
    {
        cerr << "Internal error: Unknown assembly type\n";
        return false;
    }

    return true;
}
