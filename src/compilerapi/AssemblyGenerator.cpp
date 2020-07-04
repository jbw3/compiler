#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "AssemblyGenerator.h"
#include "ErrorLogger.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;

AssemblyGenerator::AssemblyGenerator(const Config& config, ErrorLogger& logger) :
    targetMachine(config.targetMachine),
    emitType(config.emitType),
    assemblyType(config.assemblyType),
    logger(logger)
{
    if (config.outFilename.empty())
    {
        size_t idx = config.inFilename.rfind('.');
        string baseName = config.inFilename.substr(0, idx);

        if (emitType == Config::eLlvmIr)
        {
            switch (assemblyType)
            {
                case Config::eBinary:
                    outFilename = baseName + ".bc";
                    break;
                case Config::eText:
                    outFilename = baseName + ".ll";
                    break;
            }
        }
        else if (emitType == Config::eAssembly)
        {
            switch (assemblyType)
            {
                case Config::eBinary:
                    outFilename = baseName + ".o";
                    break;
                case Config::eText:
                    outFilename = baseName + ".s";
                    break;
            }
        }
        else
        {
            assert(false && "Unknown emit type");
        }
    }
    else
    {
        outFilename = config.outFilename;
    }
}

bool AssemblyGenerator::Generate(Module* module)
{
    error_code ec;
    raw_fd_ostream outFile(outFilename, ec, sys::fs::F_None);
    if (ec)
    {
        logger.LogError("Could not open output file");
        return false;
    }

    if (emitType == Config::eLlvmIr)
    {
        if (assemblyType == Config::eBinary)
        {
            // write LLVM bytecode
            WriteBitcodeToFile(*module, outFile);
        }
        else if (assemblyType == Config::eText)
        {
            // print LLVM IR
            module->print(outFile, nullptr);
        }
    }
    else if (emitType == Config::eAssembly)
    {
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        legacy::PassManager passManager;
        CodeGenFileType fileType = (assemblyType == Config::eBinary)
                                    ? CGFT_ObjectFile
                                    : CGFT_AssemblyFile;
        if (targetMachine->addPassesToEmitFile(passManager, outFile, nullptr, fileType))
        {
            logger.LogError("Target machine cannot emit a file of this type");
            return false;
        }

        passManager.run(*module);
    }
    else
    {
        logger.LogInternalError("Unknown emit type");
        return false;
    }

    return true;
}
