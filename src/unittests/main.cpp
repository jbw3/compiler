#include "LexicalAnalyzerTests.h"
#include <iostream>
#include <memory>

using namespace std;

int main()
{
    vector<shared_ptr<TestClass>> testClasses =
    {
        shared_ptr<TestClass>(new LexicalAnalyzerTests()),
    };

    bool passed = true;
    for (auto testClass : testClasses)
    {
        passed &= testClass->Run();
    }

    return passed ? 0 : 1;
}
