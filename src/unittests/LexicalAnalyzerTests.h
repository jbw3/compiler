#ifndef LEXICAL_ANALYZER_TESTS_H_
#define LEXICAL_ANALYZER_TESTS_H_

#include "Token.h"
#include <vector>

class LexicalAnalyzerTests
{
public:
    bool Run();

private:
    bool TestValidInputs();

    bool TokensAreEqual(const Token& token1, const Token& token2);

    bool TokenSequencesAreEqual(const std::vector<Token>& sequence1,
                                const std::vector<Token>& sequence2);
};

#endif // LEXICAL_ANALYZER_TESTS_H_
