#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "AssemblyGenerator.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

AssemblyGenerator::AssemblyGenerator(const Config& config) :
    targetMachine(config.targetMachine),
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
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        error_code ec;
        raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
        if (ec)
        {
            cerr << ec.message();
            return false;
        }

        legacy::PassManager passManager;
        CodeGenFileType fileType = (assemblyType == Config::eMachineBinary)
                                    ? CGFT_ObjectFile
                                    : CGFT_AssemblyFile;
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
