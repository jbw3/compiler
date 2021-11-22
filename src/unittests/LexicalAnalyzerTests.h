#ifndef LEXICAL_ANALYZER_TESTS_H_
#define LEXICAL_ANALYZER_TESTS_H_

#include "TestClass.h"
#include "Token.h"
#include "TokenList.h"

class LexicalAnalyzerTests : public TestClass
{
public:
    LexicalAnalyzerTests(std::ostream& results);

private:
    static bool TestValidInputs(std::string& failMsg);

    static bool TestNumbers(std::string& failMsg);

    static bool TestStrings(std::string& failMsg);

    static bool TokensAreEqual(const Token& token1, const Token& token2, std::string& failMsg);

    static bool TokenSequencesAreEqual(const TokenList& sequence1,
                                       const TokenList& sequence2,
                                       std::string& failMsg);
};

#endif // LEXICAL_ANALYZER_TESTS_H_
