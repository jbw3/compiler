#include "Compiler.h"
#include "SourceGenerator.h"
#include "SourceGeneratorTests.h"
#include "SyntaxTree.h"
#include <filesystem>
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

    ok = CompareFiles(inFilename, outFilename, failMsg);

    delete syntaxTree;

    return ok;
}
