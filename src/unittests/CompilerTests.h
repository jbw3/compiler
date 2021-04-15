#ifndef COMPILER_TESTS_H_
#define COMPILER_TESTS_H_

#include "TestClass.h"
#include <string>
#include <vector>

class CompilerTests : public TestClass
{
public:
    CompilerTests();

private:
    static bool RunTest(const std::string& baseFilename, bool debugInfo);

    static bool RunTest(const std::vector<std::string>& baseFilenames, bool debugInfo);
};

#endif // COMPILER_TESTS_H_
