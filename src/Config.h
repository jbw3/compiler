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
    std::string outFilename;

    Config();

    bool ParseArgs(int argc, const char* const argv[]);

private:
    bool ParseNextArgs(int& idx, int argc, const char* const argv[]);
};

#endif // CONFIG_H_
