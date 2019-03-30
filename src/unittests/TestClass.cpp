#include "TestClass.h"

using namespace std;

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
