#include "Compiler.h"
#include "Config.h"

int main(int argc, const char* const argv[])
{
    Config config;
    bool ok = config.ParseArgs(argc, argv);

    if (ok)
    {
        Compiler compiler(config);
        compiler.Compile();
    }

    return ok ? 0 : 1;
}
