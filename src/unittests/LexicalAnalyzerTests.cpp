#include "LexicalAnalyzerTests.h"
#include "ErrorLogger.h"
#include "LexicalAnalyzer.h"
#include <iostream>
#include <sstream>

using namespace std;

LexicalAnalyzerTests::LexicalAnalyzerTests()
{
    AddTest(TestValidInputs);
}

bool LexicalAnalyzerTests::TestValidInputs()
{
    vector<pair<string, vector<Token>>> tests =
    {
        {
            "fun test() i32 1 + 2\n",
            {
                {"fun", 1, 1},
                {"test", 1, 5},
                {"(", 1, 9},
                {")", 1, 10},
                {"i32", 1, 12},
                {"1", 1, 16},
                {"+", 1, 18},
                {"2", 1, 20},
                {"\n", 1, 21}
            }
        },
        {
            "fun a_function(param1 i32, param2 i32) i32 param1 - param2\n",
            {
                {"fun", 1, 1},
                {"a_function", 1, 5},
                {"(", 1, 15},
                {"param1", 1, 16},
                {"i32", 1, 23},
                {",", 1, 26},
                {"param2", 1, 28},
                {"i32", 1, 35},
                {")", 1, 38},
                {"i32", 1, 40},
                {"param1", 1, 44},
                {"-", 1, 51},
                {"param2", 1, 53},
                {"\n", 1, 59}
            }
        }
    };

    stringstream errStream;
    ErrorLogger logger(&errStream);
    LexicalAnalyzer analyzer(logger);

    stringstream ss;
    vector<Token> actualTokens;
    bool ok = false;
    for (pair<string, vector<Token>> test : tests)
    {
        errStream.clear();
        errStream.str("");

        ss.clear();
        ss.str(test.first);
        const vector<Token>& expectedTokens = test.second;

        ok = analyzer.Process(ss, actualTokens);
        if (ok)
        {
            ok = TokenSequencesAreEqual(expectedTokens, actualTokens);
        }
        if (ok)
        {
            ok = (errStream.str().size() == 0);
            if (!ok)
            {
                cerr << errStream.str();
            }
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
