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

bool SyntaxAnalyzerTests::RunSyntaxAnalysis(const string& input, string& failMsg)
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

bool SyntaxAnalyzerTests::TestInvalid(string& failMsg)
{
    vector<InvalidTest> tests =
    {
        // unexpected token
        {
            "in",
            "error: Unexpected token 'in'",
        },

        // invalid struct definitions
        {
            "const s = struct",
            "error: Unexpected end of file. Expected '{'",
        },
        {
            "const s = struct .",
            "error: Expected '{'",
        },
        {
            "const s = struct {",
            "error: Unexpected end of file. Expected '}'",
        },
        {
            "const s = struct { 123 };",
            "error: Invalid member name '123'",
        },
        {
            "const s = struct { x",
            "error: Unexpected end of file. Expected a member type",
        },
        {
            "const s = struct { x / };",
            "error: Unexpected term '/'",
        },
        {
            "const s = struct { x i32 = ",
            "error: Unexpected end of file. Expected a default member value expression",
        },
        {
            "const s = struct { x i32 = / };",
            "error: Unexpected term '/'",
        },

        // invalid struct initializations
        {
            "const s = S:",
            "error: Unexpected end of file. Expected '{' after ':'",
        },
        {
            "const s = S: /",
            "error: Expected '{' after ':'",
        },
        {
            "const s = S: {",
            "error: Unexpected end of file. Expected struct initialization",
        },
        {
            "const s = S: { 111 };",
            "error: Invalid member name '111'",
        },
        {
            "const s = S: { x",
            "error: Unexpected end of file. Expected '=' after member",
        },
        {
            "const s = S: { x / };",
            "error: Expected '=' after member",
        },
        {
            "const s = S: { x = / };",
            "error: Unexpected term '/'",
        },
        {
            "const s = S: { x = 2,",
            "error: Unexpected end of file. Expected '}'",
        },

        // invalid const declarations
        {
            "const",
            "error: Unexpected end of file. Expected a constant name",
        },
        {
            "const i32",
            "error: Invalid constant name 'i32'",
        },
        {
            "const asdf",
            "error: Unexpected end of file. Expected constant type or assignment operator",
        },
        {
            "const asdf in",
            "error: Unexpected term 'in'",
        },
        {
            "const asdf i32",
            "error: Expected another expression term",
        },
        {
            "const asdf i32 =",
            "error: Unexpected end of file. Expected expression",
        },
        {
            "const asdf i32 = ;",
            "error: Expected another expression term",
        },

        // invalid var declarations
        {
            "fun f() { var",
            "error: Unexpected end of file. Expected a variable name",
        },
        {
            "fun f() { var i32 }",
            "error: Invalid variable name 'i32'",
        },
        {
            "fun f() { var asdf",
            "error: Unexpected end of file. Expected variable type or assignment operator",
        },
        {
            "fun f() { var asdf in }",
            "error: Unexpected term 'in'",
        },
        {
            "fun f() { var asdf i32",
            "error: Expected another expression term",
        },
        {
            "fun f() { var asdf i32 =",
            "error: Unexpected end of file. Expected expression",
        },
        {
            "fun f() { var asdf i32 = ; }",
            "error: Expected another expression term",
        },

        // invalid while loops
        {
            "fun f() { while",
            "error: Expected another expression term",
        },
        {
            "fun f() { while /",
            "error: Unexpected term '/'",
        },

        // invalid for loops
        {
            "fun f() { for",
            "error: Unexpected end of file. Expected a variable name",
        },
        {
            "fun f() { for /",
            "error: Invalid iterator variable name '/'",
        },
        {
            "fun f() { for i",
            "error: Unexpected end of file. Expected variable type, ',', or 'in' keyword",
        },
        {
            "fun f() { for i /",
            "error: Unexpected term '/'",
        },
        {
            "fun f() { for i,",
            "error: Unexpected end of file. Expected variable name",
        },
        {
            "fun f() { for i, /",
            "error: Invalid index variable name '/'",
        },
        {
            "fun f() { for i, j",
            "error: Unexpected end of file. Expected variable type or 'in' keyword",
        },
        {
            "fun f() { for i, j /",
            "error: Unexpected term '/'",
        },
        {
            "fun f() { for i in",
            "error: Unexpected end of file. Expected expression",
        },
        {
            "fun f() { for i in /",
            "error: Unexpected term '/'",
        },
        {
            "fun f() { for i in x {",
            "error: Unexpected end of file. Expected block end",
        },

        // invalid arrays
        {
            "fun f() { var x = [",
            "error: Unexpected end of file",
        },
        {
            "fun f() { var x = [ in ];",
            "error: Unexpected term 'in'",
        },
        {
            "fun f() { var x = [ 1 ;",
            "error: Unexpected end of file. Expected an expression",
        },
        {
            "fun f() { var x = [ 1 ; in ];",
            "error: Unexpected term 'in'",
        },
        {
            "fun f() { var x = [ 1 ,",
            "error: Unexpected end of file. Expected an expression or ']'",
        },
        {
            "fun f() { var x = [ 1, in ]; }",
            "error: Unexpected term 'in'",
        },

        // invalid function types
        {
            "const f = fun",
            "error: Unexpected end of file. Expected '('",
        },
        {
            "const f = fun in",
            "error: Expected '('",
        },
        {
            "const f = fun(",
            "error: Unexpected end of file. Expected the rest of the function type",
        },
        {
            "const f = fun(in);",
            "error: Invalid parameter name 'in'",
        },
        {
            "const f = fun(abc",
            "error: Unexpected end of file. Expected a type",
        },
        {
            "const f = fun(abc in);",
            "error: Unexpected term 'in'",
        },

        // invalid functions
        {
            "fun",
            "error: Unexpected end of file. Expected function name",
        },
        {
            "fun (){}",
            "error: Expected function name",
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
            "error: Invalid parameter name '12'",
        },
        {
            "fun abc(x",
            "error: Unexpected end of file. Expected a parameter type",
        },
        {
            "fun abc(x /){}",
            "error: Unexpected term '/'",
        },
        {
            "fun abc(x =){}",
            "error: Unexpected term '='",
        },
        {
            "fun abc()",
            "error: Unexpected end of file. Expected a return type",
        },
        {
            "fun abc(x i32) / {}",
            "error: Unexpected term '/'",
        },
        {
            "fun abc() i32",
            "error: Expected another expression term",
        },
        {
            "fun abc() {",
            "error: Unexpected end of file. Expected block end",
        },

        // invalid extern functions
        {
            "extern",
            "error: Unexpected end of file. Expected function keyword",
        },
        {
            "extern if",
            "error: Expected function keyword",
        },
        {
            "extern fun",
            "error: Unexpected end of file. Expected function name",
        },
        {
            "extern fun (){}",
            "error: Expected function name",
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
            "error: Expected ';'",
        },
        {
            "fun f() { while true { continue } }",
            "error: Expected ';'",
        },

        // function call
        {
            "fun f() { abc(",
            "error: Unexpected end of file. Expected the rest of the function call",
        },
        {
            "fun f() { abc(,);",
            "error: Expected another expression term",
        },

        // invalid member name
        {
            "fun f() i32 { return x.",
            "error: Unexpected end of file. Expected member name identifier",
        },
        {
            "fun f() i32 { return x./; }",
            "error: Expected member name identifier",
        },

        // invalid brackets
        {
            "fun f() i32 { return x[/]; }",
            "error: Unexpected term '/'",
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
