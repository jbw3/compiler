#include "TestClass.h"

using namespace std;

TestClass::TestClass(ostream& results) :
    results(results)
{
}

bool TestClass::Run()
{
    bool allPassed = true;

    for (TestFunc test : tests)
    {
        allPassed &= test();
    }

    return allPassed;
}

void TestClass::AddTest(TestFunc test)
{
    tests.push_back(test);
}
