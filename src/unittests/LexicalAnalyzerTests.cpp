#include "LexicalAnalyzerTests.h"
#include "LexicalAnalyzer.h"
#include <sstream>

using namespace std;

bool LexicalAnalyzerTests::Run()
{
    bool passed = true;

    passed &= TestValidInputs();

    return passed;
}

bool LexicalAnalyzerTests::TestValidInputs()
{
    stringstream ss;
    vector<Token> expectedTokens;
    vector<Token> actualTokens;

    LexicalAnalyzer analyzer;

    ss.str("fun test() 1 + 2\n");
    expectedTokens = {Token("fun", 1, 1), Token("test", 1, 5), Token("(", 1, 9),
                      Token(")", 1, 10),  Token("1", 1, 12),   Token("+", 1, 14),
                      Token("2", 1, 16),  Token("\n", 1, 17)};
    bool ok = analyzer.Process(ss, actualTokens);
    if (ok)
    {
        ok = TokenSequencesAreEqual(expectedTokens, actualTokens);
    }

    return ok;
}

bool LexicalAnalyzerTests::TokensAreEqual(const Token& token1, const Token& token2)
{
    return token1.GetValue() == token2.GetValue();
}

bool LexicalAnalyzerTests::TokenSequencesAreEqual(const std::vector<Token>& sequence1,
                                                  const std::vector<Token>& sequence2)
{
    if (sequence1.size() != sequence2.size())
    {
        return false;
    }

    for (size_t i = 0; i < sequence1.size(); ++i)
    {
        bool areEqual = TokensAreEqual(sequence1[i], sequence2[i]);
        if (!areEqual)
        {
            return false;
        }
    }

    return true;
}
