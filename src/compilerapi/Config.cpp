#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "Config.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"
#include <cstring>
#include <iostream>
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

using namespace llvm;
using namespace std;

Config::Config()
{
    emitType = eAssembly;
    assemblyType = eBinary;
    outFilename = "";
    optimizationLevel = 0;
    architecture = "";
    targetMachine = CreateTargetMachine(architecture, optimizationLevel);
    debugInfo = false;
    boundsCheck = true;
    color = eAuto;
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

    if (ok)
    {
        targetMachine = CreateTargetMachine(architecture, optimizationLevel);
        if (targetMachine == nullptr)
        {
            cerr << "Error: Unknown architecture '" << architecture << "'\n";
            ok = false;
        }
    }

    return ok;
}

TargetMachine* Config::CreateTargetMachine(const std::string& architecture, unsigned optimization)
{
    // default target triple to the current machine
    Triple targetTripple(sys::getDefaultTargetTriple());

    // override the architecture if configured
    if (!architecture.empty())
    {
        Triple::ArchType archType = Triple::getArchTypeForLLVMName(architecture);
        targetTripple.setArch(archType);
    }

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();

    string errorMsg;
    const Target* target = TargetRegistry::lookupTarget(targetTripple.str(), errorMsg);
    if (target == nullptr)
    {
        cerr << errorMsg;
        return nullptr;
    }

    TargetOptions options;
    auto relocModel = Optional<Reloc::Model>();
    CodeGenOpt::Level codeGenOptLevel = CodeGenOpt::Default;
    switch (optimization)
    {
        case 0:
            codeGenOptLevel = CodeGenOpt::None;
            break;
        case 1:
            codeGenOptLevel = CodeGenOpt::Less;
            break;
        case 2:
        default:
            codeGenOptLevel = CodeGenOpt::Default;
            break;
    }

    TargetMachine* targetMachine = target->createTargetMachine(targetTripple.str(), "generic", "", options, relocModel, llvm::None, codeGenOptLevel);

    return targetMachine;
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
    else if (strcmp(arg, "-e") == 0 || strcmp(arg, "--emit") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            const char* const emit = argv[idx + 1];
            ++idx;

            if (strcmp(emit, "tokens") == 0)
            {
                emitType = eTokens;
            }
            else if (strcmp(emit, "tree") == 0)
            {
                emitType = eSyntaxTree;
            }
            else if (strcmp(emit, "c-header") == 0)
            {
                emitType = eCHeader;
            }
            else if (strcmp(emit, "llvm") == 0)
            {
                emitType = eLlvmIr;
            }
            else if (strcmp(emit, "asm") == 0)
            {
                emitType = eAssembly;
            }
            else
            {
                cerr << "Error: Unknown argument '" << emit << "'\n";
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
    else if (strcmp(arg, "-t") == 0 || strcmp(arg, "--text") == 0)
    {
        assemblyType = eText;
    }
    else if (strcmp(arg, "-d") == 0 || strcmp(arg, "--debug-info") == 0)
    {
        debugInfo = true;
    }
    else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
    {
        help = true;
        PrintHelp();
    }
    else if (strcmp(arg, "--color") == 0)
    {
        if (idx + 1 >= argc)
        {
            cerr << "Error: Expected an argument after " << arg << "\n";
            ok = false;
        }
        else
        {
            const char* const colorArg = argv[idx + 1];
            ++idx;

            if (strcmp(colorArg, "false") == 0)
            {
                color = eFalse;
            }
            else if (strcmp(colorArg, "true") == 0)
            {
                color = eTrue;
            }
            else if (strcmp(colorArg, "auto") == 0)
            {
                color = eAuto;
            }
            else
            {
                cerr << "Error: Unknown argument '" << colorArg << "'\n";
                ok = false;
            }
        }
    }
    else if (strcmp(arg, "--no-bounds-check") == 0)
    {
        boundsCheck = false;
    }
    // check for unknown options (but don't catch '-' because
    // that is used to denote reading from stdin)
    else if (arg[0] == '-' && arg[1] != '\0')
    {
        cerr << "Error: Unknown option '" << arg << "'\n";
        ok = false;
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
  --color <value>        Whether to color output messages: auto, true, false
  -d, --debug-info       Generate debug info
  -e, --emit <value>     Output type: asm, c-header, llvm, tokens, tree
  --no-bounds-check      Disable array index bounds checking
  -O <value>             Optimization level: 0, 1, 2
  -o, --output <file>    Specify name of output file
  -t, --text             Output assembly as text

Examples:

Compile test.wip to an object file:
  wip test.wip

Compile test.wip to an object file with optimization enabled:
  wip test.wip -O 2

Output LLVM IR as text:
  wip test.wip --emit llvm -t

Output a C/C++ header file:
  wip test.wip --emit c-header -o test.h

More info: https://github.com/jbw3/compiler/wiki/Compiler
)";
}
