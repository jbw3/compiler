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

bool SemanticAnalyzerTests::TestValidConstants(std::string &failMsg)
{
    vector<string> tests =
    {
        // multiple const types
        "const Num1 u64 = 1_234_567;\n"
        "const Num2 = Num1 * 3;\n"
        "const Float f32 = 3.14159265;\n"
        "const Bool = true;\n"
        "const A = struct { a i32, b f64, c bool, };\n"
        "const StructA = A { a: 12, b: 1.5, c: true, };\n",

        // structs can be evaluated out-of-order
        "const NUM1 i32 = NUM2 - 1;\n"
        "const NUM2 i32 = NUM3 - 1;\n"
        "const NUM3 i32 = 3;\n",
    };

    bool ok = false;
    for (string test : tests)
    {
        Config config;
        config.color = Config::eFalse;
        stringstream errStream;
        CompilerContext compilerContext(config, errStream);
        TokenList tokens;
        ModuleDefinition* syntaxTree = nullptr;

        CharBuffer buff;
        buff.ptr = test.c_str();
        buff.size = test.size();

        errStream.clear();
        errStream.str("");

        LexicalAnalyzer lexicalAnalyzer(compilerContext);
        ok = lexicalAnalyzer.Process(buff, tokens);
        if (!ok)
        {
            failMsg = errStream.str();
        }

        if (ok)
        {
            SyntaxAnalyzer syntaxAnalyzer(compilerContext);
            ok = syntaxAnalyzer.ProcessModule(0, tokens, syntaxTree);
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

        if (!ok)
        {
            break;
        }
    }

    return ok;
}
