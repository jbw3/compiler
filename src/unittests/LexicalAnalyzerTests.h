#ifndef LEXICAL_ANALYZER_TESTS_H_
#define LEXICAL_ANALYZER_TESTS_H_

#include "TestClass.h"
#include "Token.h"
#include "TokenList.h"

class LexicalAnalyzerTests : public TestClass
{
public:
    LexicalAnalyzerTests();

private:
    static bool TestValidInputs();

    static bool TestNumbers();

    static bool TestStrings();

    static bool TokensAreEqual(const Token& token1, const Token& token2);

    static bool TokenSequencesAreEqual(const TokenList& sequence1,
                                       const TokenList& sequence2);
};

#endif // LEXICAL_ANALYZER_TESTS_H_
