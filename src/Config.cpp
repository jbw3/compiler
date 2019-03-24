#include "Config.h"
#include <cstring>
#include <iostream>

using namespace std;

Config::Config()
{
    outputType = eAssembly;
    assemblyType = eMachineBinary;
    outFilename = "";
}

bool Config::ParseArgs(int argc, const char* const argv[])
{
    bool ok = true;
    for (int idx = 1; idx < argc; ++idx)
    {
        ok = ParseNextArgs(idx, argc, argv);
        if (!ok)
        {
            break;
        }
    }

    return ok;
}

bool Config::ParseNextArgs(int& idx, int argc, const char* const argv[])
{
    bool ok = true;
    const char* const arg = argv[idx];

    if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            outFilename = argv[idx + 1];
            ++idx;
        }
    }
    else if (strcmp(arg, "--out-type") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            const char* const outType = argv[idx + 1];
            ++idx;

            if (strcmp(outType, "tokens") == 0)
            {
                outputType = eTokens;
            }
            else if (strcmp(outType, "tree") == 0)
            {
                outputType = eSyntaxTree;
            }
            else if (strcmp(outType, "assembly") == 0)
            {
                outputType = eAssembly;
            }
            else
            {
                cerr << "Error: Unknown argument :" << outType << "\n";
                ok = false;
            }
        }
    }
    else if (strcmp(arg, "-S") == 0)
    {
        assemblyType = eMachineText;
    }
    else if (strcmp(arg, "--llvm") == 0)
    {
        assemblyType = eLlvmIr;
    }
    else
    {
        if (inFilename.empty())
        {
            inFilename = arg;
        }
        else
        {
            cerr << "Error: More than one input file cannot be specified\n";
            ok = false;
        }
    }

    return ok;
}
