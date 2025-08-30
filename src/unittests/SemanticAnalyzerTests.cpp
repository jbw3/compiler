#include "SemanticAnalyzerTests.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include <sstream>

using namespace std;
using namespace SyntaxTree;

struct InvalidTest
{
    string input;
    string expectedErrMsg;
};

SemanticAnalyzerTests::SemanticAnalyzerTests(ostream &results) :
    TestClass("SemanticAnalyzer", results)
{
    ADD_TEST(TestValidConstants);
    ADD_TEST(TestInvalidConstants);
    ADD_TEST(TestInvalidVariables);
}

bool SemanticAnalyzerTests::RunSemanticAnalysis(const string& input, string& failMsg, bool check_const_decls)
{
    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);
    compilerContext.InitBasicTypes();

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

        if (ok)
        {
            size_t totalConstDecls = syntaxTree->modules[0]->constantDeclarations.size();
            for (const FunctionDefinition* funcDef : syntaxTree->modules[0]->functionDefinitions)
            {
                const BlockExpression* blockExpr = dynamic_cast<const BlockExpression*>(funcDef->expression);
                totalConstDecls += blockExpr->constantDeclarations.size();
            }

            if (check_const_decls && totalConstDecls == 0)
            {
                ok = false;
                failMsg = "No constant declarations were generated";
            }
        }
    }

    if (ok)
    {
        SemanticAnalyzer semanticAnalyzer(compilerContext);
        ok = semanticAnalyzer.Process(syntaxTree);
        if (!ok)
        {
            failMsg = errStream.str();
        }
    }

    delete syntaxTree;
    syntaxTree = nullptr;

    return ok;
}

bool SemanticAnalyzerTests::TestValidConstants(string &failMsg)
{
    vector<string> tests =
    {
        // multiple const types
        "const Num1 u64 = 1_234_567;\n"
        "const Num2 = Num1 * 3;\n"
        "const Float f32 = 3.14159265;\n"
        "const Bool = true;\n"
        "const STR = \"abc\";\n"
        "const A = struct { a i32, b f64, c bool, };\n"
        "const B = struct { x A, s str };\n"
        "const StructA = A { a: 12, b: 1.5, c: true, };\n"
        "const StructB = B { x: StructA, s: STR };\n",

        // consts can be evaluated out-of-order
        "const NUM1 i32 = NUM2 - 1;\n"
        "const NUM2 i32 = NUM3 - 1;\n"
        "const NUM3 i32 = 3;\n",

        // structs can be defined out-of-order
        "const C = struct { b B };\n"
        "const A = struct { x i32 };\n"
        "const B = struct { a A };\n",

        // empty struct definition
        "const Empty = struct { };",

        // defining structs in an array
        R"(
        const STRUCTS []type = [
            struct { n i64 },
            struct { f f64 },
        ];
        const Struct0 type = STRUCTS[0];
        const Wrapper = struct
        {
            s Struct0
        };
        const w = Wrapper
        {
            s: Struct0
            {
                n: 123
            }
        };
        )",

        // same struct can have different names
        "const A = struct { x i32, };\n"
        "const B type = A;\n"
        "const AInst A = B { x: 123, };\n",
    };

    bool ok = false;
    for (string test : tests)
    {
        // test if constants work in the global scope
        ok = RunSemanticAnalysis(test, failMsg);
        if (!ok)
        {
            break;
        }

        // test if constants work in a function
        string test2 = "fun f()\n{\n" + test + "}\n";
        ok = RunSemanticAnalysis(test2, failMsg);
        if (!ok)
        {
            break;
        }
    }

    return ok;
}

bool SemanticAnalyzerTests::TestInvalidConstants(string &failMsg)
{
    vector<InvalidTest> tests =
    {
        // recursive dependency
        {
            "const NUM1 i32 = NUM2 - 1;\n"
            "const NUM2 i32 = NUM3 - 1;\n"
            "const NUM3 i32 = NUM1 + 2;\n",
            "error: Constant 'NUM1' has a recursive dependency on itself",
        },

        // struct dependency on self
        {
            "const B = struct { b B };\n",
            "error: Constant 'B' has a recursive dependency on itself",
        },

        // struct recursive dependency
        {
            "const A = struct { b B };\n"
            "const B = struct { c C };\n"
            "const C = struct { a A };\n",
            "error: Constant 'A' has a recursive dependency on itself",
        },

        // invalid struct type
        {
            "const I32 = i32;\n"
            "const Bad = I32 { a: 2 };\n",
            "error: Expression value is not a struct type",
        },
    };

    bool ok = true;
    bool valid = true;
    string errMsg;
    for (InvalidTest test : tests)
    {
        // check if tests fail in the global scope
        valid = RunSemanticAnalysis(test.input, errMsg);
        if (valid)
        {
            ok = false;
            failMsg = "Expected semantic analysis to fail";
            break;
        }
        if (errMsg.find(test.expectedErrMsg) == string::npos)
        {
            ok = false;
            failMsg = "Unexpected error message: " + errMsg;
            break;
        }

        // check if tests fail in a function
        string test2 = "fun f()\n{\n" + test.input + "}\n";
        valid = RunSemanticAnalysis(test2, errMsg);
        if (valid)
        {
            ok = false;
            failMsg = "Expected semantic analysis to fail";
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

bool SemanticAnalyzerTests::TestInvalidVariables(string& failMsg)
{
    vector<InvalidTest> tests =
    {
        // can't assign struct definition to var
        {
            "var s = struct { a i32, b i32 };",
            "error: Variable cannot be of type 'type'",
        },
    };

    bool ok = true;
    bool valid = true;
    string errMsg;
    for (InvalidTest test : tests)
    {
        // create function for test
        string test2 = "fun f()\n{\n" + test.input + "}\n";
        valid = RunSemanticAnalysis(test2, errMsg, /*check_const_decls =*/false);
        if (valid)
        {
            ok = false;
            failMsg = "Expected semantic analysis to fail";
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
