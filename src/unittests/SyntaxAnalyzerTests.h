#ifndef SYNTAX_ANALYZER_TESTS_H_
#define SYNTAX_ANALYZER_TESTS_H_

#include "TestClass.h"

class SyntaxAnalyzerTests : public TestClass
{
public:
    SyntaxAnalyzerTests(std::ostream& results);

private:
    static bool RunSyntaxAnalysis(const std::string& input, std::string& failMsg);

    static bool TestInvalid(std::string& failMsg);
};

#endif // SYNTAX_ANALYZER_TESTS_H_
