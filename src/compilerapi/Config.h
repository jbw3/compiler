#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

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
    std::string architecture;

    Config();

    bool ParseArgs(int argc, const char* const argv[], bool& help);

private:
    bool ParseNextArgs(int argc, const char* const argv[], int& idx, bool& help);

    void PrintHelp() const;
};

#endif // CONFIG_H_
