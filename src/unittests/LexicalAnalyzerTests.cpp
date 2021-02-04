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
    AddTest(TestNumbers);
    AddTest(TestStrings);
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

bool LexicalAnalyzerTests::TestNumbers()
{
    vector<tuple<const char*, const char*, bool, Token::EType, int64_t>> tests =
    {
        // valid numbers
        make_tuple("0", "0", true, Token::eDecIntLit, 0),
        make_tuple("0 ", "0", true, Token::eDecIntLit, 0),
        make_tuple("5\n", "5", true, Token::eDecIntLit, 5),
        make_tuple("10", "10", true, Token::eDecIntLit, 10),
        make_tuple("12 ", "12", true, Token::eDecIntLit, 12),
        make_tuple("123", "123", true, Token::eDecIntLit, 123),
        make_tuple("1234567890", "1234567890", true, Token::eDecIntLit, 1234567890),
        make_tuple("1_234", "1_234", true, Token::eDecIntLit, 1234),
        make_tuple("1_", "1_", true, Token::eDecIntLit, 1),
        make_tuple("1_1", "1_1", true, Token::eDecIntLit, 11),
        make_tuple("0b0", "0b0", true, Token::eBinIntLit, 0b0),
        make_tuple("0b1", "0b1", true, Token::eBinIntLit, 0b1),
        make_tuple("0b1 ", "0b1", true, Token::eBinIntLit, 0b1),
        make_tuple("0b101", "0b101", true, Token::eBinIntLit, 0b101),
        make_tuple("0b_1_0_1_0", "0b_1_0_1_0", true, Token::eBinIntLit, 0b1010),
        make_tuple("0b_____010_____", "0b_____010_____", true, Token::eBinIntLit, 0b010),
        make_tuple("0o0", "0o0", true, Token::eOctIntLit, 00),
        make_tuple("0o4", "0o4", true, Token::eOctIntLit, 04),
        make_tuple("0o7", "0o7", true, Token::eOctIntLit, 07),
        make_tuple("0o23", "0o23", true, Token::eOctIntLit, 023),
        make_tuple("0o1702", "0o1702", true, Token::eOctIntLit, 01702),
        make_tuple("0o716_012", "0o716_012", true, Token::eOctIntLit, 0716012),
        make_tuple("0x4", "0x4", true, Token::eHexIntLit, 0x4),
        make_tuple("0xa", "0xa", true, Token::eHexIntLit, 0xa),
        make_tuple("0xE", "0xE", true, Token::eHexIntLit, 0xE),
        make_tuple("0x10Ea5C", "0x10Ea5C", true, Token::eHexIntLit, 0x10Ea5C),
        make_tuple("0x10ea_5C7a_39c8_", "0x10ea_5C7a_39c8_", true, Token::eHexIntLit, 0x10ea5C7a39c8),

        // invalid numbers
        make_tuple("0b", "", false, Token::eInvalid, 0),
        make_tuple("0o", "", false, Token::eInvalid, 0),
        make_tuple("0x", "", false, Token::eInvalid, 0),
        make_tuple("1b", "", false, Token::eInvalid, 0),
        make_tuple("2o", "", false, Token::eInvalid, 0),
        make_tuple("3x", "", false, Token::eInvalid, 0),
        make_tuple("0b_", "", false, Token::eInvalid, 0),
        make_tuple("0o__", "", false, Token::eInvalid, 0),
        make_tuple("0x___", "", false, Token::eInvalid, 0),
        make_tuple("23a", "", false, Token::eInvalid, 0),
        make_tuple("0b120", "", false, Token::eInvalid, 0),
        make_tuple("0B10", "", false, Token::eInvalid, 0),
        make_tuple("0B", "", false, Token::eInvalid, 0),
        make_tuple("0o138", "", false, Token::eInvalid, 0),
        make_tuple("0O34", "", false, Token::eInvalid, 0),
        make_tuple("0O", "", false, Token::eInvalid, 0),
        make_tuple("0x23G4", "", false, Token::eInvalid, 0),
        make_tuple("0X19A", "", false, Token::eInvalid, 0),
        make_tuple("0X", "", false, Token::eInvalid, 0),
        make_tuple("0_b", "", false, Token::eInvalid, 0),
        make_tuple("0_o", "", false, Token::eInvalid, 0),
        make_tuple("0_X", "", false, Token::eInvalid, 0),

        // valid non-number tokens
        make_tuple("_", "_", true, Token::eIdentifier, 0),
        make_tuple("___", "___", true, Token::eIdentifier, 0),
        make_tuple("_0b", "_0b", true, Token::eIdentifier, 0),
        make_tuple("_0o", "_0o", true, Token::eIdentifier, 0),
        make_tuple("_0X", "_0X", true, Token::eIdentifier, 0),
        make_tuple("_1", "_1", true, Token::eIdentifier, 0),
    };

    CompilerContext compilerContext;
    stringstream errStream;
    ErrorLogger logger(compilerContext, &errStream, Config::eFalse);
    LexicalAnalyzer analyzer(compilerContext, logger);

    stringstream ss;
    vector<Token> tokens;
    bool ok = true;
    for (tuple<const char*, const char*, bool, Token::EType, int64_t> test : tests)
    {
        const char* input = get<0>(test);
        const char* expectedOutput = get<1>(test);
        bool expectedIsValid = get<2>(test);
        Token::EType expectedTokenType = get<3>(test);

        errStream.clear();
        errStream.str("");

        ss.clear();
        ss.str(input);

        bool actualIsValid = analyzer.Process(ss, tokens);

        if (expectedIsValid != actualIsValid)
        {
            ok = false;
            if (expectedIsValid)
            {
                cerr << "Expected input '" << input << "' to be valid\n";
                cerr << errStream.str();
            }
            else
            {
                cerr << "Expected input '" << input << "' to be invalid\n";
            }
        }

        if (expectedIsValid)
        {
            if (ok && tokens.size() != 1)
            {
                ok = false;
                cerr << "Expected 1 token but got " << tokens.size() << "\n";
            }

            if (ok)
            {
                Token expectedToken(expectedOutput, 0, 1, 1, expectedTokenType);
                const Token& actualToken = tokens[0];

                ok = TokensAreEqual(expectedToken, actualToken);
            }
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TestStrings()
{
    vector<tuple<const char*, bool>> tests =
    {
        // valid strings
        make_tuple(R"("")", true),
        make_tuple(R"("A")", true),
        make_tuple(R"("abc")", true),
        make_tuple(R"("Testing 1, 2, 3...")", true),
        make_tuple(R"("abc\n123")", true),

        // invalid strings
        make_tuple(R"(")", false),
        make_tuple(R"("abc)", false),
        make_tuple(R"("abc"")", false),
    };

    CompilerContext compilerContext;
    stringstream errStream;
    ErrorLogger logger(compilerContext, &errStream, Config::eFalse);
    LexicalAnalyzer analyzer(compilerContext, logger);

    stringstream ss;
    vector<Token> tokens;
    bool ok = true;
    for (tuple<const char*, bool> test : tests)
    {
        const char* input = get<0>(test);
        bool expectedIsValid = get<1>(test);

        errStream.clear();
        errStream.str("");

        ss.clear();
        ss.str(input);

        bool actualIsValid = analyzer.Process(ss, tokens);

        if (expectedIsValid != actualIsValid)
        {
            ok = false;
            if (expectedIsValid)
            {
                cerr << "Expected input '" << input << "' to be valid\n";
                cerr << errStream.str();
            }
            else
            {
                cerr << "Expected input '" << input << "' to be invalid\n";
            }
        }

        if (expectedIsValid)
        {
            if (ok && tokens.size() != 1)
            {
                ok = false;
                cerr << "Expected 1 token but got " << tokens.size() << "\n";
            }

            if (ok)
            {
                Token expectedToken(input, 0, 1, 1, Token::eStrLit);
                const Token& actualToken = tokens[0];

                ok = TokensAreEqual(expectedToken, actualToken);
            }
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
