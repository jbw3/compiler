#ifndef SEMANTIC_ANALYSIS_TESTS_H_
#define SEMANTIC_ANALYSIS_TESTS_H_

#include "TestClass.h"

class SemanticAnalyzerTests : public TestClass
{
public:
    SemanticAnalyzerTests(std::ostream& results);

private:
    static bool RunSemanticAnalysis(const std::string& input, std::string& failMsg);

    static bool TestValidConstants(std::string& failMsg);
};

#endif // SEMANTIC_ANALYSIS_TESTS_H_
