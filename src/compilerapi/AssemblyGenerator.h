#ifndef ASSEMBLY_GENERATOR_H_
#define ASSEMBLY_GENERATOR_H_

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "Config.h"
#pragma clang diagnostic pop

namespace llvm
{
class Module;
}

class AssemblyGenerator
{
public:
    AssemblyGenerator(const Config& config);

    bool Generate(llvm::Module* module);

private:
    std::string outFilename;
    std::string architecture;
    Config::EAssemblyType assemblyType;
};

#endif // ASSEMBLY_GENERATOR_H_
