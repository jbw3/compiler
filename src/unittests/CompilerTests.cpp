#include "CompilerTests.h"
#include "Compiler.h"
#include <fstream>
#include <iostream>

using namespace std;

CompilerTests::CompilerTests()
{
    AddTest([](){ return RunTest("basic"); });
}

bool CompilerTests::RunTest(const string& baseFilename)
{
    string testFilesDir = "src/unittests/testfiles/";
    string inFilename = testFilesDir + baseFilename + ".wip";
    string outFilename = testFilesDir + baseFilename + ".out.ll";
    string expectedFilename = testFilesDir + baseFilename + ".expected.ll";

    Config config;
    config.assemblyType = Config::eLlvmIr;
    config.inFilename = inFilename;
    config.outFilename = outFilename;

    Compiler compiler(config);
    bool ok = compiler.Compile();
    if (!ok)
    {
        return false;
    }

    fstream expectedFile(expectedFilename);
    fstream outFile(outFilename);

    unsigned long lineNum = 1;
    string expectedLine;
    string outLine;
    while (!expectedFile.eof() && !outFile.eof())
    {
        getline(expectedFile, expectedLine);
        getline(outFile, outLine);

        if (expectedLine != outLine)
        {
            cerr << "Error: Line " << lineNum << " is not correct\n";
            return false;
        }

        ++lineNum;
    }

    return true;
}
