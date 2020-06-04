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
            "fun test() i32 { 1 + 2 }\n",
            {
                {"fun", 1, 1},
                {"test", 1, 5},
                {"(", 1, 9},
                {")", 1, 10},
                {"i32", 1, 12},
                {"{", 1, 16},
                {"1", 1, 18},
                {"+", 1, 20},
                {"2", 1, 22},
                {"}", 1, 24}
            }
        },
        {
            "fun a_function(param1 i32, param2 i32) i32 { param1 - param2 }\n",
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
                {"{", 1, 44},
                {"param1", 1, 46},
                {"-", 1, 53},
                {"param2", 1, 55},
                {"}", 1, 62}
            }
        },
        {
            "1 + 23\n# comment\nnot a comment\n",
            {
                {"1", 1, 1},
                {"+", 1, 3},
                {"23", 1, 5},
                {"not", 3, 1},
                {"a", 3, 5},
                {"comment", 3, 7}
            }
        },
        {
            "    # comment\n    not a comment\n",
            {
                {"not", 2, 5},
                {"a", 2, 9},
                {"comment", 2, 11}
            }
        },
        {
            "1 + #! comment !# 2",
            {
                {"1", 1, 1},
                {"+", 1, 3},
                {"2", 1, 19},
            }
        },
        {
            "abc #! comment #! nested comment !# back in outer comment !# ()",
            {
                {"abc", 1, 1},
                {"(", 1, 62},
                {")", 1, 63},
            }
        },
        {
            "x=#! multi-\nline\ncomment\n!# true;",
            {
                {"x", 1, 1},
                {"=", 1, 2},
                {"true", 4, 4},
                {";", 4, 8},
            }
        },
        {
            "x = #!# 1 !# 2",
            {
                {"x", 1, 1},
                {"=", 1, 3},
                {"2", 1, 14},
            }
        },
        {
            R"("abc""1\n2\n""#""#!")",
            {
                {R"("abc")", 1, 1},
                {R"("1\n2\n")", 1, 6},
                {R"("#")", 1, 14},
                {R"("#!")", 1, 17},
            }
        },
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

        if (!ok)
        {
            break;
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TokensAreEqual(const Token& expectedToken, const Token& actualToken)
{
    bool areEqual = true;

    const string& expectedValue = expectedToken.GetValue();
    const string& actualValue = actualToken.GetValue();
    if (expectedValue != actualValue)
    {
        cerr << "Token values are not equal: expected: " << expectedValue << ", actual: " << actualValue << "\n";
        areEqual = false;
    }

    unsigned long expectedLine = expectedToken.GetLine();
    unsigned long actualLine = actualToken.GetLine();
    if (expectedLine != actualLine)
    {
        cerr << "Token line numbers are not equal: expected: " << expectedLine << ", actual: " << actualLine << "\n";
        areEqual = false;
    }

    unsigned long expectedColumn = expectedToken.GetColumn();
    unsigned long actualColumn = actualToken.GetColumn();
    if (expectedColumn != actualColumn)
    {
        cerr << "Token column numbers are not equal: expected: " << expectedColumn << ", actual: " << actualColumn << "\n";
        areEqual = false;
    }

    return areEqual;
}

bool LexicalAnalyzerTests::TokenSequencesAreEqual(const std::vector<Token>& expectedTokens,
                                                  const std::vector<Token>& actualTokens)
{
    if (expectedTokens.size() != actualTokens.size())
    {
        return false;
    }

    for (size_t i = 0; i < expectedTokens.size(); ++i)
    {
        bool areEqual = TokensAreEqual(expectedTokens[i], actualTokens[i]);
        if (!areEqual)
        {
            return false;
        }
    }

    return true;
}
