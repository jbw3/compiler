#ifndef ASSEMBLY_GENERATOR_H_
#define ASSEMBLY_GENERATOR_H_

#include "Config.h"

namespace llvm
{
class Module;
class TargetMachine;
}
class ErrorLogger;

class AssemblyGenerator
{
public:
    AssemblyGenerator(const Config& config, ErrorLogger& logger);

    bool Generate(llvm::Module* module);

private:
    std::string outFilename;
    llvm::TargetMachine* targetMachine;
    Config::EEmitType emitType;
    Config::EAssemblyType assemblyType;
    ErrorLogger& logger;
};

#endif // ASSEMBLY_GENERATOR_H_
