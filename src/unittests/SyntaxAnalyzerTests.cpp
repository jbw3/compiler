#include "SyntaxAnalyzerTests.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include <sstream>

using namespace std;
using namespace SyntaxTree;

struct InvalidTest
{
    string input;
    string expectedErrMsg;
};

SyntaxAnalyzerTests::SyntaxAnalyzerTests(ostream &results) :
    TestClass("SyntaxAnalyzer", results)
{
    ADD_TEST(TestInvalid);
}

bool SyntaxAnalyzerTests::RunSyntaxAnalysis(const std::string& input, std::string& failMsg)
{
    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);

    Modules* syntaxTree = new Modules;

    char* input_copy = new char[input.size()]; // this will be freed by CompilerContext
    memcpy(input_copy, input.c_str(), input.size());
    CharBuffer buff;
    buff.ptr = input_copy;
    buff.size = input.size();

    compilerContext.AddFile("test", buff);
    TokenList& tokens = compilerContext.GetFileTokens(0);

    LexicalAnalyzer lexicalAnalyzer(compilerContext);
    bool ok = lexicalAnalyzer.Process(buff, tokens);
    if (!ok)
    {
        failMsg = errStream.str();
    }

    if (ok && tokens.GetSize() == 0)
    {
        ok = false;
        failMsg = "No tokens were generated";
    }

    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer(compilerContext);
        ok = syntaxAnalyzer.Process(syntaxTree);
        if (!ok)
        {
            failMsg = errStream.str();
        }

        if (ok && syntaxTree->modules.size() == 0)
        {
            ok = false;
            failMsg = "No modules were generated";
        }
    }

    delete syntaxTree;
    syntaxTree = nullptr;

    return ok;
}

bool SyntaxAnalyzerTests::TestInvalid(std::string& failMsg)
{
    vector<InvalidTest> tests =
    {
        // unexpected end of file
        {
            "const s = struct",
            "error: Unexpected end of file. Expected '{'",
        },

        // invalid functions
        {
            "fun",
            "error: Unexpected end of file. Expected function name",
        },
        {
            "fun (){}",
            "error: '(' is not a valid function name",
        },
        {
            "fun abc",
            "error: Unexpected end of file. Expected '('",
        },
        {
            "fun x{}",
            "error: Expected '('",
        },
        {
            "fun abc(",
            "error: Unexpected end of file. Expected ')'",
        },
        {
            "fun abc(12){}",
            "error: Invalid parameter name: '12'",
        },
        {
            "fun abc(x",
            "error: Unexpected end of file. Expected a parameter type",
        },
        {
            "fun abc(x /){}",
            "error: Unexpected term '/",
        },
        {
            "fun abc()",
            "error: Unexpected end of file. Expected a return type",
        },
        {
            "fun abc(x i32) / {}",
            "error: Unexpected term '/",
        },

        // invalid strings
        {
            "const x = \"\\q\";",
            "error: Invalid escape sequence",
        },
        {
            "const x = \"\xff\";",
            "error: Invalid character in string",
        },

        // invalid \x string escape sequences
        {
            "const x = \"\\x\";",
            "error: Reached end of string before end of '\\x' escape sequence",
        },
        {
            "const x = \"\\xy\";",
            "error: Invalid hexadecimal digit in '\\x' escape sequence",
        },
        {
            "const x = \"\\xff\";",
            "error: Invalid UTF-8 byte in '\\x' escape sequence",
        },

        // invalid \u{} string escape sequences
        {
            "const x = \"\\u\";",
            "error: Reached end of string before end of '\\u' escape sequence",
        },
        {
            "const x = \"\\u000a\";",
            "error: Expected '{' after '\\u'",
        },
        {
            "const x = \"\\u{\";",
            "error: Reached end of string before end of '\\u' escape sequence",
        },
        {
            "const x = \"\\u{}\";",
            "error: Unexpected '}' after '{'",
        },
        {
            "const x = \"\\u{123456789}\";",
            "error: '\\u' escape sequence cannot contain more than 8 digits",
        },
        {
            "const x = \"\\u{1x}\";",
            "error: Invalid hexadecimal digit in '\\u' escape sequence",
        },
        {
            "const x = \"\\u{1a\";",
            "error: Reached end of string before end of '\\u' escape sequence",
        },
        {
            "const x = \"\\u{12345678}\";",
            "error: Unicode sequence exceeds max value",
        },

        // no ';' after break/continue
        {
            "fun f() { while true { break } }",
            "error: Expected ';' after 'break'",
        },
        {
            "fun f() { while true { continue } }",
            "error: Expected ';' after 'continue'",
        },

    };

    bool ok = true;
    bool valid = true;
    string errMsg;
    for (InvalidTest test : tests)
    {
        valid = RunSyntaxAnalysis(test.input, errMsg);
        if (valid)
        {
            ok = false;
            failMsg = "Expected syntax analysis to fail";
            break;
        }
        if (errMsg.find(test.expectedErrMsg) == string::npos)
        {
            ok = false;
            failMsg = "Unexpected error message: " + errMsg;
            break;
        }
    }

    return ok;
}
