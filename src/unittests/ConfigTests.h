#ifndef CONFIG_TESTS_H_
#define CONFIG_TESTS_H_

#include "TestClass.h"
#include <string>

class ConfigTests : public TestClass
{
public:
    ConfigTests(std::ostream& results);

private:

    static bool TestParseArgs(std::string& failMsg);
};

#endif // CONFIG_TESTS_H_
