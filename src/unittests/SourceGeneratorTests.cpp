#include "Compiler.h"
#include "SourceGenerator.h"
#include "SourceGeneratorTests.h"
#include "SyntaxTree.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace std;

SourceGeneratorTests::SourceGeneratorTests(ostream& results) :
    TestClass("SourceGenerator", results)
{
    ADD_TEST(TestSourceGenerator);
}

bool SourceGeneratorTests::TestSourceGenerator(string& failMsg)
{
    stringstream err;

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
        err << "Error: " << inFilename << ": Failed to compile\n";
        failMsg = err.str();
        return false;
    }

    if (syntaxTree == nullptr)
    {
        err << "Error: " << inFilename << ": syntax tree pointer was null\n";
        failMsg = err.str();
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
            err << "Error: " << outFilename << ": Line " << lineNum << " is not correct\n"
                << expectedLine << '\n'
                << outLine << '\n';
            failMsg = err.str();
            return false;
        }

        ++lineNum;
    }

    if (expectedFile.eof() != outFile.eof())
    {
        err << "Error: " << outFilename << ": Unexpected number of lines\n";
        failMsg = err.str();
        return false;
    }

    delete syntaxTree;

    return true;
}
