#ifndef UTILS_TEST_H_
#define UTILS_TEST_H_

#include "TestClass.h"

class UtilsTests : public TestClass
{
public:
    UtilsTests(std::ostream& results);

private:
    static bool TestNumberConversion();
};

#endif // UTILS_TEST_H_
