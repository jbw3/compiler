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
    ADD_TEST(TestValidVariables);
    ADD_TEST(TestInvalidVariables);
}

bool SemanticAnalyzerTests::RunSemanticAnalysis(const string& input, string& failMsg, bool check_const_decls)
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
        "const StructA = A: { a = 12, b = 1.5, c = true, };\n"
        "const StructB = B: { x = StructA, s = STR };\n",

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

        // struct with default member values
        "const X i32 = 12;\n"
        "const S = struct { m1 bool = true, m2 i32 = X + 2, m3 bool, m4 str = \"\", m5 i32 };",

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
        const w = Wrapper:
        {
            s = Struct0:
            {
                n = 123
            }
        };
        )",

        // same struct can have different names
        "const A = struct { x i32, };\n"
        "const B type = A;\n"
        "const AInst A = B: { x = 123, };\n",

        // struct can have a pointer to itself
        "const A = struct { a &A };",
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

bool SemanticAnalyzerTests::TestInvalidConstants(string& failMsg)
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
            "error: Member 'b' creates a recursive dependency",
        },

        // struct recursive dependency
        {
            "const A = struct { b B };\n"
            "const B = struct { c C };\n"
            "const C = struct { a A };\n",
            "error: Member 'b' creates a recursive dependency",
        },

        // struct recursive dependency via array
        {
            "const A = struct { a []A };",
            "error: Member 'a' creates a recursive dependency",
        },

        // invalid struct type
        {
            "const I32 = i32;\n"
            "const Bad = I32: { a = 2 };\n",
            "error: Expression value is not a struct type",
        },

        // assigning a struct of the wrong type
        {
            "const A = struct { a i32, b f64, c bool };\n"
            "const B = struct { a i32, b f64, c bool };\n"
            "const AInst A = B: { a = 123, b = 1.23, c = false };\n",
            "error: Binary operator '=' does not support types 'A' and 'B'",
        },

        // struct with invalid default member value type
        {
            "const X i32 = 12;\n"
            "const S = struct { m1 bool = X };",
            "error: Default value type 'i32' cannot be assigned to member type 'bool'",
        },

        // struct with non-constant default member value
        {
            "const S = struct { m1 bool = true && true };",
            "error: Default member value is not a constant expression",
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

bool SemanticAnalyzerTests::TestValidVariables(string& failMsg)
{
    vector<string> tests =
    {
        // constants in different scopes can have the same name
        "{ const A = struct { x i8, y f32 }; var a = A: { x = 3, y = 9.2 }; }\n"
        "{ const A = struct { x i16, b bool }; var a = A: { x = 3_000, b = true }; }\n",

        // built-in identifiers
        "var x f32 = @e;\n"
        "var y f64 = @e + @pi * x;\n",
    };

    bool ok = false;
    for (string test : tests)
    {
        string test2 = "fun f()\n{\n" + test + "}\n";
        ok = RunSemanticAnalysis(test2, failMsg, /*check_const_decls =*/false);
        if (!ok)
        {
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

        // str can't be initialized like a struct
        {
            "const S = str;\n"
            "var s = S: { };\n",
            "error: Expression value is not a struct type",
        },

        // invalid built-in identifiers
        {
            "var x = @notValid;\n",
            "error: '@notValid' is not a valid built-in identifier",
        },

        // invalid bit-cast expression type
        {
            "var a = @bitCast(u8, true);",
            "error: Cannot bit-cast expression of type 'bool' to type 'u8'",
        },

        // invalid bit-cast cast type
        {
            "var x u8 = 1;\n"
            "var a = @bitCast(bool, x);\n",
            "error: Cannot bit-cast expression of type 'u8' to type 'bool'",
        },

        // can't bit-cast int literal to int type that's too small
        {
            "var a = @bitCast(u8, 17_000);",
            "error: Cannot bit-cast expression of type '{integer}' to type 'u8' because sizes are not equal",
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
