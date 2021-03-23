#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

namespace llvm
{
class TargetMachine;
}

class Config
{
public:
    enum EEmitType
    {
        eTokens,
        eSyntaxTree,
        eCHeader,
        eLlvmIr,
        eAssembly,
    };

    enum EAssemblyType
    {
        eBinary,
        eText,
    };

    enum EColor
    {
        eFalse,
        eTrue,
        eAuto,
    };

    EEmitType emitType;
    EAssemblyType assemblyType;
    std::vector<std::string> inFilenames;
    std::string outFilename;
    unsigned optimizationLevel;
    std::string architecture;
    llvm::TargetMachine* targetMachine;
    bool debugInfo;
    bool boundsCheck;
    EColor color;

    Config();

    bool ParseArgs(int argc, const char* const argv[], bool& help);

private:
    static llvm::TargetMachine* CreateTargetMachine(const std::string& architecture, unsigned optimization);

    bool ParseNextArgs(int argc, const char* const argv[], int& idx, bool& help);

    void PrintHelp() const;
};

#endif // CONFIG_H_
