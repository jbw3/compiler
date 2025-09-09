#ifndef SEMANTIC_ANALYSIS_TESTS_H_
#define SEMANTIC_ANALYSIS_TESTS_H_

#include "TestClass.h"

class SemanticAnalyzerTests : public TestClass
{
public:
    SemanticAnalyzerTests(std::ostream& results);

private:
    static bool RunSemanticAnalysis(const std::string& input, std::string& failMsg, bool check_const_decls = true);

    static bool TestValidConstants(std::string& failMsg);

    static bool TestInvalidConstants(std::string& failMsg);

    static bool TestValidVariables(std::string& failMsg);

    static bool TestInvalidVariables(std::string& failMsg);
};

#endif // SEMANTIC_ANALYSIS_TESTS_H_
