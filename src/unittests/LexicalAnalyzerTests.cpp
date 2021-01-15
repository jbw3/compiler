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
                {"fun", "", 1, 1, Token::eInvalid},
                {"test", "", 1, 5, Token::eInvalid},
                {"(", "", 1, 9, Token::eInvalid},
                {")", "", 1, 10, Token::eInvalid},
                {"i32", "", 1, 12, Token::eInvalid},
                {"{", "", 1, 16, Token::eInvalid},
                {"1", "", 1, 18, Token::eInvalid},
                {"+", "", 1, 20, Token::eInvalid},
                {"2", "", 1, 22, Token::eInvalid},
                {"}", "", 1, 24, Token::eInvalid}
            }
        },
        {
            "fun a_function(param1 i32, param2 i32) i32 { param1 - param2 }\n",
            {
                {"fun", "", 1, 1, Token::eInvalid},
                {"a_function", "", 1, 5, Token::eInvalid},
                {"(", "", 1, 15, Token::eInvalid},
                {"param1", "", 1, 16, Token::eInvalid},
                {"i32", "", 1, 23, Token::eInvalid},
                {",", "", 1, 26, Token::eInvalid},
                {"param2", "", 1, 28, Token::eInvalid},
                {"i32", "", 1, 35, Token::eInvalid},
                {")", "", 1, 38, Token::eInvalid},
                {"i32", "", 1, 40, Token::eInvalid},
                {"{", "", 1, 44, Token::eInvalid},
                {"param1", "", 1, 46, Token::eInvalid},
                {"-", "", 1, 53, Token::eInvalid},
                {"param2", "", 1, 55, Token::eInvalid},
                {"}", "", 1, 62, Token::eInvalid}
            }
        },
        {
            "1 + 23\n# comment\nnot a comment\n",
            {
                {"1", "", 1, 1, Token::eInvalid},
                {"+", "", 1, 3, Token::eInvalid},
                {"23", "", 1, 5, Token::eInvalid},
                {"not", "", 3, 1, Token::eInvalid},
                {"a", "", 3, 5, Token::eInvalid},
                {"comment", "", 3, 7, Token::eInvalid}
            }
        },
        {
            "    # comment\n    not a comment\n",
            {
                {"not", "", 2, 5, Token::eInvalid},
                {"a", "", 2, 9, Token::eInvalid},
                {"comment", "", 2, 11, Token::eInvalid}
            }
        },
        {
            "1 + #! comment !# 2",
            {
                {"1", "", 1, 1, Token::eInvalid},
                {"+", "", 1, 3, Token::eInvalid},
                {"2", "", 1, 19, Token::eInvalid},
            }
        },
        {
            "abc #! comment #! nested comment !# back in outer comment !# ()",
            {
                {"abc", "", 1, 1, Token::eInvalid},
                {"(", "", 1, 62, Token::eInvalid},
                {")", "", 1, 63, Token::eInvalid},
            }
        },
        {
            "x=#! multi-\nline\ncomment\n!# true;",
            {
                {"x", "", 1, 1, Token::eInvalid},
                {"=", "", 1, 2, Token::eInvalid},
                {"true", "", 4, 4, Token::eInvalid},
                {";", "", 4, 8, Token::eInvalid},
            }
        },
        {
            "x = #!# 1 !# 2",
            {
                {"x", "", 1, 1, Token::eInvalid},
                {"=", "", 1, 3, Token::eInvalid},
                {"2", "", 1, 14, Token::eInvalid},
            }
        },
        {
            R"("abc""1\n2\n""#""#!")",
            {
                {R"("abc")", "", 1, 1, Token::eInvalid},
                {R"("1\n2\n")", "", 1, 6, Token::eInvalid},
                {R"("#")", "", 1, 14, Token::eInvalid},
                {R"("#!")", "", 1, 17, Token::eInvalid},
            }
        },
    };

    stringstream errStream;
    ErrorLogger logger(&errStream, Config::eFalse);
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

    const string& expectedValue = expectedToken.value;
    const string& actualValue = actualToken.value;
    if (expectedValue != actualValue)
    {
        cerr << "Token values are not equal: expected: " << expectedValue << ", actual: " << actualValue << "\n";
        areEqual = false;
    }

    unsigned expectedLine = expectedToken.line;
    unsigned actualLine = actualToken.line;
    if (expectedLine != actualLine)
    {
        cerr << "Token line numbers are not equal: expected: " << expectedLine << ", actual: " << actualLine << "\n";
        areEqual = false;
    }

    unsigned expectedColumn = expectedToken.column;
    unsigned actualColumn = actualToken.column;
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
    size_t expectedSize = expectedTokens.size();
    size_t actualSize = actualTokens.size();
    if (expectedSize != actualSize)
    {
        cerr << "Unexpected number of tokens: expected: " << expectedSize << ", actual: " << actualSize << "\n";
        return false;
    }

    for (size_t i = 0; i < expectedSize; ++i)
    {
        bool areEqual = TokensAreEqual(expectedTokens[i], actualTokens[i]);
        if (!areEqual)
        {
            cerr << "Tokens are not equal\n";
            return false;
        }
    }

    return true;
}
