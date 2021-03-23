#ifdef _MSC_VER
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include "Compiler.h"
#include "SourceGenerator.h"
#include "SourceGeneratorTests.h"
#include "SyntaxTree.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::experimental::filesystem;
using namespace std;

SourceGeneratorTests::SourceGeneratorTests()
{
    AddTest(TestSourceGenerator);
}

bool SourceGeneratorTests::TestSourceGenerator()
{
    fs::path testFilesDir = fs::path("src") / "unittests" / "testfiles";
    string inFilename = (testFilesDir / "source_gen.wip").string();
    string outFilename = (testFilesDir / "source_gen.out.wip").string();

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

    SyntaxTree::Modules* syntaxTree = nullptr;
    Config config;
    config.inFilenames.push_back(inFilename);

    Compiler compiler(config);
    bool ok = compiler.CompileSyntaxTree(syntaxTree);
    if (!ok)
    {
        cerr << "Error: " << inFilename << ": Failed to compile\n";
        return false;
    }

    if (syntaxTree == nullptr)
    {
        cerr << "Error: " << inFilename << ": syntax tree pointer was null\n";
        return false;
    }

    SourceGenerator generator(outFilename);
    generator.Visit(syntaxTree);
    generator.Flush();

    fstream expectedFile(inFilename);
    fstream outFile(outFilename);

    unsigned lineNum = 1;
    string expectedLine;
    string outLine;
    while (!expectedFile.eof() && !outFile.eof())
    {
        getline(expectedFile, expectedLine);
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

    delete syntaxTree;

    return true;
}
