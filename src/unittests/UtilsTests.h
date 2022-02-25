#ifndef UTILS_TEST_H_
#define UTILS_TEST_H_

#include "TestClass.h"

class UtilsTests : public TestClass
{
public:
    UtilsTests(std::ostream& results);

private:
    static bool TestIntConversion(std::string& failMsg);

    static bool TestFloatConversion(std::string& failMsg);
};

#endif // UTILS_TEST_H_
