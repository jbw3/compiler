#include "Compiler.h"
#include "Config.h"

int main(int argc, const char* const argv[])
{
    Config config;
    bool help = false;
    bool ok = config.ParseArgs(argc, argv, help);

    if (ok && !help)
    {
        Compiler compiler(config);
        ok = compiler.Compile();
    }

    return ok ? 0 : 1;
}
