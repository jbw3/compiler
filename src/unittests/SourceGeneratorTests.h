#ifndef SOURCE_GENERATOR_TESTS_H_
#define SOURCE_GENERATOR_TESTS_H_

#include "TestClass.h"

class SourceGeneratorTests : public TestClass
{
public:
    SourceGeneratorTests(std::ostream& results);

private:
    static bool TestSourceGenerator();
};

#endif // SOURCE_GENERATOR_TESTS_H_
