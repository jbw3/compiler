#ifdef _MSC_VER
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include "CompilerTests.h"
#include "Compiler.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace fs = std::experimental::filesystem;
using namespace std;

CompilerTests::CompilerTests()
{
    AddTest([](){ return RunTest("basic", false); });
    AddTest([](){ return RunTest("debug_info", true); });
}

bool CompilerTests::RunTest(const string& baseFilename, bool debugInfo)
{
    fs::path testFilesDir = fs::path("src") / "unittests" / "testfiles";
    string inFilename = (testFilesDir / (baseFilename + ".wip")).string();
    string outFilename = (testFilesDir / (baseFilename + ".out.ll")).string();
    string expectedFilename = (testFilesDir / (baseFilename + ".expected.ll")).string();

    string directory = fs::current_path().string();
#ifdef _WIN32
    // escape backslashes
    size_t idx = directory.find('\\');
    while (idx != string::npos)
    {
        directory.insert(idx, 1, '\\');

        idx = directory.find('\\', idx + 2);
    }
#endif

    unordered_map<string, string> lineMap =
    {
#ifdef _WIN32
        { "module_id", "; ModuleID = 'src\\unittests\\testfiles\\" + baseFilename + ".wip'" },
        { "source_filename", "source_filename = \"src\\\\unittests\\\\testfiles\\\\" + baseFilename + ".wip\"" },
        { "target_datalayout", "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"" },
        { "target_triple", "target triple = \"x86_64-pc-windows-msvc\"" },
        { "filename", "!1 = !DIFile(filename: \"src\\\\unittests\\\\testfiles\\\\debug_info.wip\", directory: \"" + directory + "\")" },
#else
        { "module_id", "; ModuleID = 'src/unittests/testfiles/" + baseFilename + ".wip'" },
        { "source_filename", "source_filename = \"src/unittests/testfiles/" + baseFilename + ".wip\"" },
        { "target_datalayout", "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"" },
        { "target_triple", "target triple = \"x86_64-pc-linux-gnu\"" },
        { "filename", "!1 = !DIFile(filename: \"src/unittests/testfiles/debug_info.wip\", directory: \"" + directory + "\")" },
#endif
    };

    Config config;
    config.emitType = Config::eLlvmIr;
    config.assemblyType = Config::eText;
    config.inFilename = inFilename;
    config.outFilename = outFilename;
    config.debugInfo = debugInfo;

    Compiler compiler(config);
    bool ok = compiler.Compile();
    if (!ok)
    {
        cerr << "Error: " << inFilename << ": Failed to compile\n";
        return false;
    }

    fstream expectedFile(expectedFilename);
    fstream outFile(outFilename);

    unsigned lineNum = 1;
    string expectedLine;
    string outLine;
    while (!expectedFile.eof() && !outFile.eof())
    {
        getline(expectedFile, expectedLine);
        if (expectedLine.size() > 0 && expectedLine[0] == '$')
        {
            string key = expectedLine.substr(1);
            auto iter = lineMap.find(key);
            if (iter != lineMap.end())
            {
                expectedLine = iter->second;
            }
        }

        getline(outFile, outLine);

        if (expectedLine != outLine)
        {
            cerr << "Error: " << outFilename << ": Line " << lineNum << " is not correct\n"
                 << expectedLine << '\n'
                 << outLine << '\n';
            return false;
        }

        ++lineNum;
    }

    if (expectedFile.eof() != outFile.eof())
    {
        cerr << "Error: " << outFilename << ": Unexpected number of lines\n";
        return false;
    }

    return true;
}
