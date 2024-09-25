#include "CompilerTests.h"
#include "LexicalAnalyzerTests.h"
#include "ROStringTests.h"
#include "SemanticAnalyzerTests.h"
#include "SourceGeneratorTests.h"
#include "UtilsTests.h"
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

int main(int argc, const char* const argv[])
{
    fstream resultsFile("test_results.xml", ios_base::out);
    resultsFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuites>\n";

    vector<shared_ptr<TestClass>> testClasses =
    {
        shared_ptr<TestClass>(new LexicalAnalyzerTests(resultsFile)),
        shared_ptr<TestClass>(new SemanticAnalyzerTests(resultsFile)),
        shared_ptr<TestClass>(new CompilerTests(resultsFile)),
        shared_ptr<TestClass>(new ROStringTests(resultsFile)),
        shared_ptr<TestClass>(new SourceGeneratorTests(resultsFile)),
        shared_ptr<TestClass>(new UtilsTests(resultsFile)),
    };

    bool passed = true;

    // if there are no arguments, run all tests
    if (argc <= 1)
    {
        for (auto testClass : testClasses)
        {
            passed &= testClass->Run();
        }
    }
    // otherwise, just run the ones specified
    else
    {
        for (auto testClass : testClasses)
        {
            for (int i = 1; i < argc; ++i)
            {
                if (testClass->GetName() == argv[i])
                {
                    passed &= testClass->Run();
                }
            }
        }
    }

    resultsFile << "</testsuites>\n";
    resultsFile.close();

    return passed ? 0 : 1;
}
