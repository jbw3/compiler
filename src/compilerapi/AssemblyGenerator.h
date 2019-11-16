#ifndef ASSEMBLY_GENERATOR_H_
#define ASSEMBLY_GENERATOR_H_

#include "Config.h"

namespace llvm
{
class Module;
class TargetMachine;
}

class AssemblyGenerator
{
public:
    AssemblyGenerator(const Config& config);

    bool Generate(llvm::Module* module);

private:
    std::string outFilename;
    llvm::TargetMachine* targetMachine;
    Config::EAssemblyType assemblyType;
};

#endif // ASSEMBLY_GENERATOR_H_
