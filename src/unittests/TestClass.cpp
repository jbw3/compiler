#include "TestClass.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

static string xmlEscape(const string& str)
{
    string newStr;
    for (char ch : str)
    {
        switch (ch)
        {
            case '\n':
                newStr += "&#10;";
                break;
            case '\r':
                newStr += "&#13;";
                break;
            case '"':
                newStr += "&quot;";
                break;
            case '&':
                newStr += "&amp;";
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

bool TestClass::CompareFiles(
    const string& expectedFilename,
    const string& actualFilename,
    string& failMsg
)
{
    fstream expectedFile(expectedFilename);
    fstream actualFile(actualFilename);

    unsigned lineNum = 1;
    string expectedLine;
    string outLine;
    while (!expectedFile.eof() && !actualFile.eof())
    {
        getline(expectedFile, expectedLine);
        getline(actualFile, outLine);

        if (expectedLine != outLine)
        {
            stringstream err;
            err << "Error: " << actualFilename << ": Line " << lineNum << " is not correct\n"
                << expectedLine << '\n'
                << outLine << '\n';
            failMsg = err.str();
            return false;
        }

        ++lineNum;
    }

    if (expectedFile.eof() != actualFile.eof())
    {
        stringstream err;
        err << "Error: " << actualFilename << ": Unexpected number of lines\n";
        failMsg = err.str();
        return false;
    }

    return true;
}

void TestClass::AddTest(const string& testName, TestFunc test)
{
    TestData data;
    data.name = testName;
    data.test = test;
    data.passed = false;

    tests.push_back(data);
}
