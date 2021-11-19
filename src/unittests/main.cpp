#include "CompilerTests.h"
#include "LexicalAnalyzerTests.h"
#include "SourceGeneratorTests.h"
#include "UtilsTests.h"
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

int main()
{
    fstream resultsFile("test_results.xml", ios_base::out);
    resultsFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuites>\n";

    vector<shared_ptr<TestClass>> testClasses =
    {
        shared_ptr<TestClass>(new LexicalAnalyzerTests(resultsFile)),
        shared_ptr<TestClass>(new CompilerTests(resultsFile)),
        shared_ptr<TestClass>(new SourceGeneratorTests(resultsFile)),
        shared_ptr<TestClass>(new UtilsTests(resultsFile)),
    };

    bool passed = true;
    for (auto testClass : testClasses)
    {
        passed &= testClass->Run();
    }

    resultsFile << "</testsuites>\n";
    resultsFile.close();

    return passed ? 0 : 1;
}
