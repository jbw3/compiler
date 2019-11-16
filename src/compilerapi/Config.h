#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

namespace llvm
{
class TargetMachine;
}

class Config
{
public:
    enum EOutputType
    {
        eTokens,
        eSyntaxTree,
        eCHeader,
        eAssembly,
    };

    enum EAssemblyType
    {
        eLlvmIr,
        eMachineText,
        eMachineBinary,
    };

    EOutputType outputType;
    EAssemblyType assemblyType;
    std::string inFilename;
    std::string outFilename;
    unsigned optimizationLevel;
    llvm::TargetMachine* targetMachine;

    Config();

    bool ParseArgs(int argc, const char* const argv[], bool& help);

private:
    static llvm::TargetMachine* CreateTargetMachine(const std::string& architecture);

    bool ParseNextArgs(int argc, const char* const argv[], int& idx, bool& help);

    void PrintHelp() const;
};

#endif // CONFIG_H_
