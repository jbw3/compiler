#include "ConfigTests.h"
#include "Config.h"
#include <string.h>

using namespace std;

static bool CheckConfigs(const Config& config1, const Config& config2)
{
    if (config1.inFilenames.size() != config2.inFilenames.size())
    {
        return false;
    }

    for (size_t i = 0; i < config1.inFilenames.size(); ++i)
    {
        if (config1.inFilenames[i] != config2.inFilenames[i])
        {
            return false;
        }
    }

    if (config1.emitType != config2.emitType)
    {
        return false;
    }
    else if (config1.assemblyType != config2.assemblyType)
    {
        return false;
    }
    else if (config1.outFilename != config2.outFilename)
    {
        return false;
    }
    else if (config1.optimizationLevel != config2.optimizationLevel)
    {
        return false;
    }
    else if (config1.architecture != config2.architecture)
    {
        return false;
    }
    else if (config1.debugInfo != config2.debugInfo)
    {
        return false;
    }
    else if (config1.assertions != config2.assertions)
    {
        return false;
    }
    else if (config1.boundsCheck != config2.boundsCheck)
    {
        return false;
    }
    else if (config1.color != config2.color)
    {
        return false;
    }

    return true;
}

ConfigTests::ConfigTests(ostream& results) :
    TestClass("Config", results)
{
    ADD_TEST(TestParseArgs);
}

struct Test
{
    vector<string> args;
    Config expectedConfig;
};

bool ConfigTests::TestParseArgs(string& failMsg)
{
    vector<Test> tests;

    Config helpConfig;
    tests.push_back({{"-h"}, helpConfig});
    tests.push_back({{"--help"}, helpConfig});

    Config outConfig;
    outConfig.inFilenames.push_back("");
    outConfig.outFilename = "out";
    tests.push_back({{"-o", "out"}, outConfig});
    tests.push_back({{"--output", "out"}, outConfig});

    Config opt1Config;
    opt1Config.inFilenames.push_back("");
    opt1Config.optimizationLevel = 1;
    tests.push_back({{"-O", "1"}, opt1Config});

    Config inputsConfig;
    inputsConfig.inFilenames = {"abc.wip", "xyz.wip", "main.wip"};
    inputsConfig.outFilename = "out";
    tests.push_back({{"abc.wip", "xyz.wip", "-o", "out", "main.wip"}, inputsConfig});

    Config emitAsmConfig;
    emitAsmConfig.inFilenames.push_back("");
    emitAsmConfig.emitType = Config::eAssembly;
    tests.push_back({{"-e", "asm"}, emitAsmConfig});
    tests.push_back({{"--emit", "asm"}, emitAsmConfig});

    Config emitCHeaderConfig;
    emitCHeaderConfig.inFilenames.push_back("");
    emitCHeaderConfig.emitType = Config::eCHeader;
    tests.push_back({{"-e", "c-header"}, emitCHeaderConfig});
    tests.push_back({{"--emit", "c-header"}, emitCHeaderConfig});

    Config emitLlvmConfig;
    emitLlvmConfig.inFilenames.push_back("");
    emitLlvmConfig.emitType = Config::eLlvmIr;
    tests.push_back({{"-e", "llvm"}, emitLlvmConfig});
    tests.push_back({{"--emit", "llvm"}, emitLlvmConfig});

    Config emitTokensConfig;
    emitTokensConfig.inFilenames.push_back("");
    emitTokensConfig.emitType = Config::eTokens;
    tests.push_back({{"-e", "tokens"}, emitTokensConfig});
    tests.push_back({{"--emit", "tokens"}, emitTokensConfig});

    Config emitTreeConfig;
    emitTreeConfig.inFilenames.push_back("");
    emitTreeConfig.emitType = Config::eSyntaxTree;
    tests.push_back({{"-e", "tree"}, emitTreeConfig});
    tests.push_back({{"--emit", "tree"}, emitTreeConfig});

    Config colorFalseConfig;
    colorFalseConfig.inFilenames.push_back("");
    colorFalseConfig.color = Config::eFalse;
    tests.push_back({{"--color", "false"}, colorFalseConfig});

    Config colorTrueConfig;
    colorTrueConfig.inFilenames.push_back("");
    colorTrueConfig.color = Config::eTrue;
    tests.push_back({{"--color", "true"}, colorTrueConfig});

    Config colorAutoConfig;
    colorAutoConfig.inFilenames.push_back("");
    colorAutoConfig.color = Config::eAuto;
    tests.push_back({{"--color", "auto"}, colorAutoConfig});

    Config noChecksConfig;
    noChecksConfig.inFilenames.push_back("");
    noChecksConfig.assertions = false;
    noChecksConfig.boundsCheck = false;
    tests.push_back({{"--no-assertions", "--no-bounds-check"}, noChecksConfig});

    char* argv[16];
    for (size_t i = 0; i < 16; ++i)
    {
        argv[i] = new char[32];
        memset(argv[i], 0, 32);
    }
    strcpy(argv[0], "unittests");

    bool ok = true;
    for (const Test& test : tests)
    {
        for (size_t i = 1; i < 16; ++i)
        {
            memset(argv[i], 0, 32);
        }

        int argc = test.args.size() + 1;
        for (int i = 1; i < argc; ++i)
        {
            strcpy(argv[i], test.args[i - 1].c_str());
        }

        bool help = false;
        Config config;
        ok = config.ParseArgs(argc, argv, help);
        if (!ok)
        {
            failMsg = "Could not parse args";
            break;
        }

        ok = CheckConfigs(test.expectedConfig, config);
        if (!ok)
        {
            failMsg = "Configs are not equal";
            break;
        }
    }

    for (size_t i = 0; i < 16; ++i)
    {
        delete [] argv[i];
    }

    return ok;
}
