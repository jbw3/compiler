#include "LexicalAnalyzerTests.h"
#include "CompilerContext.h"
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
                {"fun", 0, 1, 1, Token::eFun},
                {"test", 0, 1, 5, Token::eIdentifier},
                {"(", 0, 1, 9, Token::eOpenPar},
                {")", 0, 1, 10, Token::eClosePar},
                {"i32", 0, 1, 12, Token::eI32},
                {"{", 0, 1, 16, Token::eOpenBrace},
                {"1", 0, 1, 18, Token::eDecIntLit},
                {"+", 0, 1, 20, Token::ePlus},
                {"2", 0, 1, 22, Token::eDecIntLit},
                {"}", 0, 1, 24, Token::eCloseBrace}
            }
        },
        {
            "fun a_function(param1 i32, param2 i32) i32 { param1 - param2 }\n",
            {
                {"fun", 0, 1, 1, Token::eFun},
                {"a_function", 0, 1, 5, Token::eIdentifier},
                {"(", 0, 1, 15, Token::eOpenPar},
                {"param1", 0, 1, 16, Token::eIdentifier},
                {"i32", 0, 1, 23, Token::eI32},
                {",", 0, 1, 26, Token::eComma},
                {"param2", 0, 1, 28, Token::eIdentifier},
                {"i32", 0, 1, 35, Token::eI32},
                {")", 0, 1, 38, Token::eClosePar},
                {"i32", 0, 1, 40, Token::eI32},
                {"{", 0, 1, 44, Token::eOpenBrace},
                {"param1", 0, 1, 46, Token::eIdentifier},
                {"-", 0, 1, 53, Token::eMinus},
                {"param2", 0, 1, 55, Token::eIdentifier},
                {"}", 0, 1, 62, Token::eCloseBrace}
            }
        },
        {
            "1 + 23\n# comment\nnot a comment\n",
            {
                {"1", 0, 1, 1, Token::eDecIntLit},
                {"+", 0, 1, 3, Token::ePlus},
                {"23", 0, 1, 5, Token::eDecIntLit},
                {"not", 0, 3, 1, Token::eIdentifier},
                {"a", 0, 3, 5, Token::eIdentifier},
                {"comment", 0, 3, 7, Token::eIdentifier}
            }
        },
        {
            "    # comment\n    not a comment\n",
            {
                {"not", 0, 2, 5, Token::eIdentifier},
                {"a", 0, 2, 9, Token::eIdentifier},
                {"comment", 0, 2, 11, Token::eIdentifier}
            }
        },
        {
            "1 + #! comment !# 2",
            {
                {"1", 0, 1, 1, Token::eDecIntLit},
                {"+", 0, 1, 3, Token::ePlus},
                {"2", 0, 1, 19, Token::eDecIntLit},
            }
        },
        {
            "abc #! comment #! nested comment !# back in outer comment !# ()",
            {
                {"abc", 0, 1, 1, Token::eIdentifier},
                {"(", 0, 1, 62, Token::eOpenPar},
                {")", 0, 1, 63, Token::eClosePar},
            }
        },
        {
            "x=#! multi-\nline\ncomment\n!# true;",
            {
                {"x", 0, 1, 1, Token::eIdentifier},
                {"=", 0, 1, 2, Token::eEqual},
                {"true", 0, 4, 4, Token::eTrueLit},
                {";", 0, 4, 8, Token::eSemiColon},
            }
        },
        {
            "x = #!# 1 !# 2",
            {
                {"x", 0, 1, 1, Token::eIdentifier},
                {"=", 0, 1, 3, Token::eEqual},
                {"2", 0, 1, 14, Token::eDecIntLit},
            }
        },
        {
            R"("abc""1\n2\n""#""#!")",
            {
                {R"("abc")", 0, 1, 1, Token::eStrLit},
                {R"("1\n2\n")", 0, 1, 6, Token::eStrLit},
                {R"("#")", 0, 1, 14, Token::eStrLit},
                {R"("#!")", 0, 1, 17, Token::eStrLit},
            }
        },
    };

    CompilerContext compilerContext;
    stringstream errStream;
    ErrorLogger logger(compilerContext, &errStream, Config::eFalse);
    LexicalAnalyzer analyzer(compilerContext, logger);

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

    Token::EType expectedType = expectedToken.type;
    Token::EType actualType = actualToken.type;
    if (expectedType != actualType)
    {
        cerr << "Token types are not equal: expected: " << expectedType << ", actual: " << actualType << "\n";
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
