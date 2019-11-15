#include "Config.h"
#include <cstring>
#include <iostream>

using namespace std;

Config::Config()
{
    outputType = eAssembly;
    assemblyType = eMachineBinary;
    outFilename = "";
    architecture = "";
    optimizationLevel = 0;
}

bool Config::ParseArgs(int argc, const char* const argv[], bool& help)
{
    help = false;
    bool ok = true;
    for (int idx = 1; idx < argc; ++idx)
    {
        ok = ParseNextArgs(argc, argv, idx, help);
        if (!ok || help)
        {
            break;
        }
    }

    return ok;
}

bool Config::ParseNextArgs(int argc, const char* const argv[], int& idx, bool& help)
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
    else if (strcmp(arg, "-O") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            ++idx;
            const char* level = argv[idx];

            if (strcmp(level, "0") == 0)
            {
                optimizationLevel = 0;
            }
            else if (strcmp(level, "1") == 0)
            {
                optimizationLevel = 1;
            }
            else if (strcmp(level, "2") == 0)
            {
                optimizationLevel = 2;
            }
            else
            {
                cerr << "Error: Unknown optimization level " << level << "\n";
                ok = false;
            }
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
            else if (strcmp(outType, "c-header") == 0)
            {
                outputType = eCHeader;
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
    else if (strcmp(arg, "--arch") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            ++idx;
            architecture = argv[idx];
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
    else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
    {
        help = true;
        PrintHelp();
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

void Config::PrintHelp() const
{
    cout << R"(Usage: wip <filename> [options]

Options:
  -h, --help             Print help message
  --arch <value>         Assembly architecture
  --llvm                 Output LLVM IR
  --out-type <value>     Type of output: assembly, c-header, tokens, tree
  -O <value>             Optimization level: 0, 1, 2
  -o, --output <file>    Specify name of output file
  -S                     Output assembly as text

Examples:

Compile test.wip to an object file:
  wip test.wip

Compile test.wip to an object file with optimization enabled:
  wip test.wip -O 2

Output LLVM IR:
  wip test.wip --llvm

Output a C/C++ header file:
  wip test.wip --out-type c-header -o test.h
)";
}
