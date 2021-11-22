#ifndef UTILS_TEST_H_
#define UTILS_TEST_H_

#include "TestClass.h"

class UtilsTests : public TestClass
{
public:
    UtilsTests(std::ostream& results);

private:
    static bool TestNumberConversion(std::string& failMsg);
};

#endif // UTILS_TEST_H_
