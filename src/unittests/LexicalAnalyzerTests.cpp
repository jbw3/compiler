#include "LexicalAnalyzerTests.h"
#include "CompilerContext.h"
#include "ErrorLogger.h"
#include "LexicalAnalyzer.h"
#include <sstream>

using namespace std;

LexicalAnalyzerTests::LexicalAnalyzerTests(ostream& results) :
    TestClass("LexicalAnalyzer", results)
{
    ADD_TEST(TestValidInputs);
    ADD_TEST(TestNumbers);
    ADD_TEST(TestStrings);
}

bool LexicalAnalyzerTests::TestValidInputs(string& failMsg)
{
    vector<pair<string, TokenList>> tests =
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

    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);
    LexicalAnalyzer analyzer(compilerContext);

    TokenList actualTokens;
    bool ok = false;
    for (pair<string, TokenList> test : tests)
    {
        errStream.clear();
        errStream.str("");

        const TokenList& expectedTokens = test.second;

        CharBuffer buff;
        buff.size = test.first.size();
        buff.ptr = test.first.c_str();
        ok = analyzer.Process(buff, actualTokens);
        if (ok)
        {
            ok = TokenSequencesAreEqual(expectedTokens, actualTokens, failMsg);
        }
        if (ok)
        {
            ok = (errStream.str().size() == 0);
            if (!ok)
            {
                failMsg = errStream.str();
            }
        }

        if (!ok)
        {
            break;
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TestNumbers(string& failMsg)
{
    vector<tuple<const char*, const char*, bool, Token::EType>> tests =
    {
        // valid numbers
        make_tuple("0", "0", true, Token::eDecIntLit),
        make_tuple("0 ", "0", true, Token::eDecIntLit),
        make_tuple("5\n", "5", true, Token::eDecIntLit),
        make_tuple("10", "10", true, Token::eDecIntLit),
        make_tuple("12 ", "12", true, Token::eDecIntLit),
        make_tuple("123", "123", true, Token::eDecIntLit),
        make_tuple("1234567890", "1234567890", true, Token::eDecIntLit),
        make_tuple("1_234", "1_234", true, Token::eDecIntLit),
        make_tuple("1_", "1_", true, Token::eDecIntLit),
        make_tuple("1_1", "1_1", true, Token::eDecIntLit),
        make_tuple("0b0", "0b0", true, Token::eBinIntLit),
        make_tuple("0b1", "0b1", true, Token::eBinIntLit),
        make_tuple("0b1 ", "0b1", true, Token::eBinIntLit),
        make_tuple("0b101", "0b101", true, Token::eBinIntLit),
        make_tuple("0b_1_0_1_0", "0b_1_0_1_0", true, Token::eBinIntLit),
        make_tuple("0b_____010_____", "0b_____010_____", true, Token::eBinIntLit),
        make_tuple("0o0", "0o0", true, Token::eOctIntLit),
        make_tuple("0o4", "0o4", true, Token::eOctIntLit),
        make_tuple("0o7", "0o7", true, Token::eOctIntLit),
        make_tuple("0o23", "0o23", true, Token::eOctIntLit),
        make_tuple("0o1702", "0o1702", true, Token::eOctIntLit),
        make_tuple("0o716_012", "0o716_012", true, Token::eOctIntLit),
        make_tuple("0x4", "0x4", true, Token::eHexIntLit),
        make_tuple("0xa", "0xa", true, Token::eHexIntLit),
        make_tuple("0xE", "0xE", true, Token::eHexIntLit),
        make_tuple("0x10Ea5C", "0x10Ea5C", true, Token::eHexIntLit),
        make_tuple("0x10ea_5C7a_39c8_", "0x10ea_5C7a_39c8_", true, Token::eHexIntLit),
        make_tuple("1.2", "1.2", true, Token::eFloatLit),
        make_tuple("1.23", "1.23", true, Token::eFloatLit),
        make_tuple("50.9", "50.9", true, Token::eFloatLit),
        make_tuple("123.456", "123.456", true, Token::eFloatLit),
        make_tuple("0.0", "0.0", true, Token::eFloatLit),
        make_tuple("0.8", "0.8", true, Token::eFloatLit),
        make_tuple("9.0", "9.0", true, Token::eFloatLit),
        make_tuple("1_234.567_890", "1_234.567_890", true, Token::eFloatLit),
        make_tuple("1_234_._567_890", "1_234_._567_890", true, Token::eFloatLit),
        make_tuple("1e3", "1e3", true, Token::eFloatLit),
        make_tuple("1.2e7", "1.2e7", true, Token::eFloatLit),
        make_tuple("9843.32e987", "9843.32e987", true, Token::eFloatLit),
        make_tuple("1_234e3", "1_234e3", true, Token::eFloatLit),
        make_tuple("1_2_3_4_._5_6_e_7_8", "1_2_3_4_._5_6_e_7_8", true, Token::eFloatLit),

        // invalid numbers
        make_tuple("0b", "", false, Token::eInvalid),
        make_tuple("0o", "", false, Token::eInvalid),
        make_tuple("0x", "", false, Token::eInvalid),
        make_tuple("1b", "", false, Token::eInvalid),
        make_tuple("2o", "", false, Token::eInvalid),
        make_tuple("3x", "", false, Token::eInvalid),
        make_tuple("0b_", "", false, Token::eInvalid),
        make_tuple("0o__", "", false, Token::eInvalid),
        make_tuple("0x___", "", false, Token::eInvalid),
        make_tuple("23a", "", false, Token::eInvalid),
        make_tuple("0b120", "", false, Token::eInvalid),
        make_tuple("0B10", "", false, Token::eInvalid),
        make_tuple("0B", "", false, Token::eInvalid),
        make_tuple("0o138", "", false, Token::eInvalid),
        make_tuple("0O34", "", false, Token::eInvalid),
        make_tuple("0O", "", false, Token::eInvalid),
        make_tuple("0x23G4", "", false, Token::eInvalid),
        make_tuple("0X19A", "", false, Token::eInvalid),
        make_tuple("0X", "", false, Token::eInvalid),
        make_tuple("0_b", "", false, Token::eInvalid),
        make_tuple("0_o", "", false, Token::eInvalid),
        make_tuple("0_X", "", false, Token::eInvalid),
        make_tuple("1.", "", false, Token::eInvalid),
        make_tuple("1._", "", false, Token::eInvalid),
        make_tuple("1.___", "", false, Token::eInvalid),

        // valid non-number tokens
        make_tuple("_", "_", true, Token::eIdentifier),
        make_tuple("___", "___", true, Token::eIdentifier),
        make_tuple("_0b", "_0b", true, Token::eIdentifier),
        make_tuple("_0o", "_0o", true, Token::eIdentifier),
        make_tuple("_0X", "_0X", true, Token::eIdentifier),
        make_tuple("_1", "_1", true, Token::eIdentifier),
    };

    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);
    LexicalAnalyzer analyzer(compilerContext);

    TokenList tokens;
    bool ok = true;
    for (tuple<const char*, const char*, bool, Token::EType> test : tests)
    {
        const char* input = get<0>(test);
        const char* expectedOutput = get<1>(test);
        bool expectedIsValid = get<2>(test);
        Token::EType expectedTokenType = get<3>(test);

        errStream.clear();
        errStream.str("");

        CharBuffer buff;
        buff.size = strlen(input);
        buff.ptr = input;
        bool actualIsValid = analyzer.Process(buff, tokens);

        if (expectedIsValid != actualIsValid)
        {
            ok = false;
            if (expectedIsValid)
            {
                failMsg = "Expected input '"s + input + "' to be valid\n" + errStream.str();
            }
            else
            {
                failMsg = "Expected input '"s + input + "' to be invalid\n";
            }
        }

        if (expectedIsValid)
        {
            if (ok && tokens.GetSize() != 1)
            {
                ok = false;
                failMsg = "Expected 1 token but got "s + to_string(tokens.GetSize()) + "\n";
            }

            if (ok)
            {
                Token expectedToken(expectedOutput, 0, 1, 1, expectedTokenType);
                const Token& actualToken = tokens[0];

                ok = TokensAreEqual(expectedToken, actualToken, failMsg);
            }
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TestStrings(string& failMsg)
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

    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);
    LexicalAnalyzer analyzer(compilerContext);

    TokenList tokens;
    bool ok = true;
    for (tuple<const char*, bool> test : tests)
    {
        const char* input = get<0>(test);
        bool expectedIsValid = get<1>(test);

        errStream.clear();
        errStream.str("");

        CharBuffer buff;
        buff.size = strlen(input);
        buff.ptr = input;
        bool actualIsValid = analyzer.Process(buff, tokens);

        if (expectedIsValid != actualIsValid)
        {
            ok = false;
            if (expectedIsValid)
            {
                failMsg = "Expected input '"s + input + "' to be valid\n" + errStream.str();
            }
            else
            {
                failMsg = "Expected input '"s + input + "' to be invalid\n";
            }
        }

        if (expectedIsValid)
        {
            if (ok && tokens.GetSize() != 1)
            {
                ok = false;
                failMsg = "Expected 1 token but got "s + to_string(tokens.GetSize()) + "\n";
            }

            if (ok)
            {
                Token expectedToken(input, 0, 1, 1, Token::eStrLit);
                const Token& actualToken = tokens[0];

                ok = TokensAreEqual(expectedToken, actualToken, failMsg);
            }
        }
    }

    return ok;
}

bool LexicalAnalyzerTests::TokensAreEqual(const Token& expectedToken, const Token& actualToken, string& failMsg)
{
    bool areEqual = true;

    ROString expectedValue = expectedToken.value;
    ROString actualValue = actualToken.value;
    if (expectedValue != actualValue)
    {
        failMsg = "Token values are not equal: expected: "s + expectedValue.ToStdString() + ", actual: " + actualValue.ToStdString() + "\n";
        areEqual = false;
    }

    unsigned expectedLine = expectedToken.line;
    unsigned actualLine = actualToken.line;
    if (expectedLine != actualLine)
    {
        failMsg = "Token line numbers are not equal: expected: "s + to_string(expectedLine) + ", actual: " + to_string(actualLine) + "\n";
        areEqual = false;
    }

    unsigned expectedColumn = expectedToken.column;
    unsigned actualColumn = actualToken.column;
    if (expectedColumn != actualColumn)
    {
        failMsg = "Token column numbers are not equal: expected: "s + to_string(expectedColumn) + ", actual: " + to_string(actualColumn) + "\n";
        areEqual = false;
    }

    Token::EType expectedType = expectedToken.type;
    Token::EType actualType = actualToken.type;
    if (expectedType != actualType)
    {
        failMsg = "Token types are not equal: expected: "s + to_string(expectedType) + ", actual: " + to_string(actualType) + "\n";
        areEqual = false;
    }

    return areEqual;
}

bool LexicalAnalyzerTests::TokenSequencesAreEqual(const TokenList& expectedTokens,
                                                  const TokenList& actualTokens,
                                                  string& failMsg)
{
    size_t expectedSize = expectedTokens.GetSize();
    size_t actualSize = actualTokens.GetSize();
    if (expectedSize != actualSize)
    {
        failMsg = "Unexpected number of tokens: expected: "s + to_string(expectedSize) + ", actual: " + to_string(actualSize) + "\n";
        return false;
    }

    for (size_t i = 0; i < expectedSize; ++i)
    {
        bool areEqual = TokensAreEqual(expectedTokens[i], actualTokens[i], failMsg);
        if (!areEqual)
        {
            return false;
        }
    }

    return true;
}
