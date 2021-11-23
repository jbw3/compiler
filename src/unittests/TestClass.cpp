#include "TestClass.h"
#include <iostream>

using namespace std;

string xmlEscape(const string& str)
{
    string newStr;
    for (char ch : str)
    {
        switch (ch)
        {
            case '\n':
                newStr += "&#10;";
                break;
            default:
                newStr += ch;
                break;
        }
    }

    return newStr;
}

TestClass::TestClass(const string& name, ostream& results) :
    name(name),
    results(results)
{
}

bool TestClass::Run()
{
    unsigned numFailed = 0;

    // run tests
    for (TestData& testData : tests)
    {
        testData.passed = testData.test(testData.failMsg);
        if (!testData.passed)
        {
            ++numFailed;
            cerr << testData.failMsg;
        }
    }

    // print test results file
    results << "    <testsuite name=\"" << name
            << "\" tests=\"" << tests.size()
            << "\" failures=\"" << numFailed << "\">\n";

    for (const TestData& testData : tests)
    {
        results << "        <testcase name=\"" << testData.name << "\">\n";
        if (!testData.passed)
        {
            string escapedFailMsg = xmlEscape(testData.failMsg);
            results << "            <failure message=\"" << escapedFailMsg << "\"></failure>\n";
        }
        results << "        </testcase>\n";
    }

    results << "    </testsuite>\n";

    return numFailed == 0;
}

void TestClass::AddTest(const string& testName, TestFunc test)
{
    tests.push_back({.name = testName, .test = test, .passed = false});
}
