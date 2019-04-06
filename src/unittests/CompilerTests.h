#ifndef COMPILER_TESTS_H_
#define COMPILER_TESTS_H_

#include "TestClass.h"
#include <string>

class CompilerTests : public TestClass
{
public:
    CompilerTests();

private:
    static bool RunTest(const std::string& baseFilename);
};

#endif // COMPILER_TESTS_H_
