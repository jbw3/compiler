#include "CompilerTests.h"
#include "Compiler.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;

CompilerTests::CompilerTests(ostream& results) :
    TestClass("Compiler", results)
{
    AddTest("basic", [](string& failMsg){ return RunLlvmIrTest("basic", false, failMsg); });
    AddTest("debug_info", [](string& failMsg){ return RunLlvmIrTest("debug_info", true, failMsg); });
    AddTest("multi_file", [](string& failMsg)
    {
        vector<string> multiFiles = { "multi_file1", "multi_file2" };
        return RunLlvmIrTest(multiFiles, true, failMsg);
    });
    ADD_TEST(TestPrintTokens);
    ADD_TEST(TestSyntaxTreePrinter);
}

bool CompilerTests::RunLlvmIrTest(const string& baseFilename, bool debugInfo, string& failMsg)
{
    vector<string> baseFilenames;
    baseFilenames.push_back(baseFilename);
    return RunLlvmIrTest(baseFilenames, debugInfo, failMsg);
}

bool CompilerTests::RunLlvmIrTest(const vector<string>& baseFilenames, bool debugInfo, string& failMsg)
{
    string firstBaseFilename = baseFilenames.front();

    fs::path testFilesDir = fs::path("src") / "unittests" / "testfiles";
    string outFilename = (testFilesDir / (firstBaseFilename + ".out.ll")).string();
    string expectedFilename = (testFilesDir / (firstBaseFilename + ".expected.ll")).string();

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
        { "module_id", "; ModuleID = 'src\\unittests\\testfiles\\" + firstBaseFilename + ".wip'" },
        { "source_filename", "source_filename = \"src\\\\unittests\\\\testfiles\\\\" + firstBaseFilename + ".wip\"" },
        { "target_datalayout", "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128\"" },
        { "target_triple", "target triple = \"x86_64-pc-windows-msvc\"" },
        { "filename", "!1 = !DIFile(filename: \"src\\\\unittests\\\\testfiles\\\\debug_info.wip\", directory: \"" + directory + "\")" },
        { "filename1", "!1 = !DIFile(filename: \"src\\\\unittests\\\\testfiles\\\\multi_file1.wip\", directory: \"" + directory + "\")" },
        { "filename2", "!18 = !DIFile(filename: \"src\\\\unittests\\\\testfiles\\\\multi_file2.wip\", directory: \"" + directory + "\")" },
#else
        { "module_id", "; ModuleID = 'src/unittests/testfiles/" + firstBaseFilename + ".wip'" },
        { "source_filename", "source_filename = \"src/unittests/testfiles/" + firstBaseFilename + ".wip\"" },
        { "target_datalayout", "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128\"" },
        { "target_triple", "target triple = \"x86_64-pc-linux-gnu\"" },
        { "filename", "!1 = !DIFile(filename: \"src/unittests/testfiles/debug_info.wip\", directory: \"" + directory + "\")" },
        { "filename1", "!1 = !DIFile(filename: \"src/unittests/testfiles/multi_file1.wip\", directory: \"" + directory + "\")" },
        { "filename2", "!18 = !DIFile(filename: \"src/unittests/testfiles/multi_file2.wip\", directory: \"" + directory + "\")" },
#endif
    };

    Config config;
    config.emitType = Config::eLlvmIr;
    config.assemblyType = Config::eText;
    for (const string& f : baseFilenames)
    {
        string inFilename = (testFilesDir / (f + ".wip")).string();
        config.inFilenames.push_back(inFilename);
    }
    config.outFilename = outFilename;
    config.debugInfo = debugInfo;

    Compiler compiler(config);
    bool ok = compiler.Compile();
    if (!ok)
    {
        failMsg = "Error: Failed to compile\n";
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
            failMsg = "Error: "s + outFilename + ": Line " + to_string(lineNum) + " is not correct\n"
                    + expectedLine + '\n'
                    + outLine + '\n';
            return false;
        }

        ++lineNum;
    }

    if (expectedFile.eof() != outFile.eof())
    {
        failMsg = "Error: " + outFilename + ": Unexpected number of lines\n";
        return false;
    }

    return true;
}

bool CompilerTests::TestPrintTokens(string& failMsg)
{
    fs::path testFilesDir = fs::path("src") / "unittests" / "testfiles";
    string wipFilename = (testFilesDir / "tokens.wip").string();
    string expectedFilename = (testFilesDir / "tokens.expected.txt").string();
    string actualFilename = (testFilesDir / "tokens.out.txt").string();

    Config config;
    config.emitType = Config::eTokens;
    config.outFilename = actualFilename;
    config.inFilenames.push_back(wipFilename);

    Compiler compiler(config);
    bool ok = compiler.Compile();
    if (!ok)
    {
        failMsg = "Error: Failed to compile\n";
        return false;
    }

    if (ok)
    {
        ok = CompareFiles(expectedFilename, actualFilename, failMsg);
    }

    return ok;
}

bool CompilerTests::TestSyntaxTreePrinter(string& failMsg)
{
    fs::path testFilesDir = fs::path("src") / "unittests" / "testfiles";
    string wipFilename = (testFilesDir / "syntax_tree.wip").string();
    string expectedFilename = (testFilesDir / "syntax_tree.expected.json").string();
    string actualFilename = (testFilesDir / "syntax_tree.out.json").string();

    Config config;
    config.emitType = Config::eSyntaxTree;
    config.outFilename = actualFilename;
    config.inFilenames.push_back(wipFilename);

    Compiler compiler(config);
    bool ok = compiler.Compile();
    if (!ok)
    {
        failMsg = "Error: Failed to compile\n";
        return false;
    }

    if (ok)
    {
        ok = CompareFiles(expectedFilename, actualFilename, failMsg);
    }

    return ok;
}
