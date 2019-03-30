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
    vector<pair<string, vector<Token>>> tests =
    {
        {
            "fun test() 1 + 2\n",
            {
                {"fun", 1, 1},
                {"test", 1, 5},
                {"(", 1, 9},
                {")", 1, 10},
                {"1", 1, 12},
                {"+", 1, 14},
                {"2", 1, 16},
                {"\n", 1, 17}
            }
        },
        {
            "fun a_function(param1, param2) param1 - param2\n",
            {
                {"fun", 1, 1},
                {"a_function", 1, 5},
                {"(", 1, 15},
                {"param1", 1, 16},
                {",", 1, 22},
                {"param2", 1, 24},
                {")", 1, 30},
                {"param1", 1, 32},
                {"-", 1, 39},
                {"param2", 1, 41},
                {"\n", 1, 47}
            }
        }
    };


    LexicalAnalyzer analyzer;

    stringstream ss;
    vector<Token> actualTokens;
    bool ok = false;
    for (pair<string, vector<Token>> test : tests)
    {
        ss.clear();
        ss.str(test.first);
        const vector<Token>& expectedTokens = test.second;

        ok = analyzer.Process(ss, actualTokens);
        if (ok)
        {
            ok = TokenSequencesAreEqual(expectedTokens, actualTokens);
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TokensAreEqual(const Token& token1, const Token& token2)
{
    return token1.GetValue() == token2.GetValue() && token1.GetLine() == token2.GetLine() &&
           token1.GetColumn() == token2.GetColumn();
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
