#include "SemanticAnalyzerTests.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include <sstream>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzerTests::SemanticAnalyzerTests(ostream &results) :
    TestClass("SemanticAnalyzer", results)
{
    ADD_TEST(TestValidConstants);
}

bool SemanticAnalyzerTests::RunSemanticAnalysis(const string& input, string& failMsg)
{
    Config config;
    config.color = Config::eFalse;
    stringstream errStream;
    CompilerContext compilerContext(config, errStream);
    TokenList tokens;
    Modules* syntaxTree = new Modules;

    char* input_copy = new char[input.size()]; // this will be freed by CompilerContext
    memcpy(input_copy, input.c_str(), input.size());
    CharBuffer buff;
    buff.ptr = input_copy;
    buff.size = input.size();

    compilerContext.AddFile("test", buff);

    errStream.clear();
    errStream.str("");

    LexicalAnalyzer lexicalAnalyzer(compilerContext);
    bool ok = lexicalAnalyzer.Process(buff, tokens);
    if (!ok)
    {
        failMsg = errStream.str();
    }

    if (ok)
    {
        SyntaxAnalyzer syntaxAnalyzer(compilerContext);
        ok = syntaxAnalyzer.Process(syntaxTree);
        if (!ok)
        {
            failMsg = errStream.str();
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

        // structs can be evaluated out-of-order
        "const NUM1 i32 = NUM2 - 1;\n"
        "const NUM2 i32 = NUM3 - 1;\n"
        "const NUM3 i32 = 3;\n",
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
