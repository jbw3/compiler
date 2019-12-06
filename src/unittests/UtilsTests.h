#ifndef UTILS_TEST_H_
#define UTILS_TEST_H_

#include "TestClass.h"

class UtilsTests : public TestClass
{
public:
    UtilsTests();

private:
    static bool TestNumberConversion();

    static bool TestStringParsing();
};

#endif // UTILS_TEST_H_
