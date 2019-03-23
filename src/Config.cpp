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

    return ok;
}
