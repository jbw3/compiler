#include "StartEndTokenFinderTests.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "StartEndTokenFinder.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"

using namespace std;
using namespace SyntaxTree;

StartEndTokenFinderTests::StartEndTokenFinderTests(ostream &results) :
    TestClass("StartEndTokenFinder", results)
{
    ADD_TEST(Test);
}

Modules* StartEndTokenFinderTests::CreateSyntaxTree(
    stringstream& errStream,
    CompilerContext& compilerContext,
    const std::string& input,
    std::string& failMsg
)
{
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

    if (ok)
    {
        SemanticAnalyzer semanticAnalyzer(compilerContext);
        ok = semanticAnalyzer.Process(syntaxTree);
        if (!ok)
        {
            failMsg = errStream.str();
        }
    }

    if (!ok)
    {
        delete syntaxTree;
        syntaxTree = nullptr;
    }

    return syntaxTree;
}

struct TestValue
{
    string input;
    unsigned startLineOffset;
    unsigned startColumnOffset;
    unsigned endLineOffset;
    unsigned endColumnOffset;
};

bool StartEndTokenFinderTests::Test(std::string& failMsg)
{
    vector<TestValue> tests = {
        // unary
        {
            "const abc i32 = 2;\n"
            "const test = - abc;",
            2, 14, 2, 18,
        },

        // binary
        {
            "const x i32 = 4;\n"
            "const test = x + 2;",
            2, 14, 2, 18,
        },
        {
            "const xyz i32 = 12;\n"
            "const abc i32 = 30;\n"
            "const test = xyz / abc - 1_000;",
            3, 14, 3, 30,
        },
        {
            "const xyz []i32 = [20; 0];\n"
            "const test = xyz[17];",
            2, 14, 2, 20,
        },

        // function type expression
        {
            "const test = fun (a i32, b bool);",
            1, 14, 1, 32,
        },
        {
            "const test = fun (a i32, b bool) str;",
            1, 14, 1, 36,
        },
        {
            "const test = fun (a i32,\nb bool) str;",
            1, 14, 2, 11,
        },

        // numeric
        {
            "const test i16 = 0x10;",
            1, 18, 1, 21,
        },

        // float
        {
            "const test f32 = 1.234;",
            1, 18, 1, 22,
        },
        {
            "const test f64 = 0.99e-30;",
            1, 18, 1, 25,
        },

        // bool
        {
            "const test = true;",
            1, 14, 1, 17,
        },
        {
            "const test = false;",
            1, 14, 1, 18,
        },

        // string
        {
            "const test = \"abc\";",
            1, 14, 1, 18,
        },
        {
            "const test = \"123\\n456\";",
            1, 14, 1, 23,
        },

        // built-in identifier
        {
            "const test f64 = @pi;",
            1, 18, 1, 20,
        },

        // array size/value
        {
            "const test []f64 = [3; 17.5];",
            1, 20, 1, 28,
        },

        // array multi-value
        {
            "const test []i32 = [1, 2, 300];",
            1, 20, 1, 30,
        },
        {
            "const test []f32 = [\n1.2,\n@pi,\n77e3,\n];",
            1, 20, 5, 1,
        },

        // implicit cast
        {
            "const abc i8 = 1;\n"
            "const test i32 = abc + 1;",
            2, 18, 2, 24,
        },

        // built-in function call
        {
            "const a i32 = 12;\n"
            "const test = @cast(f32, a);",
            2, 14, 2, 26,
        },
    };

    Modules* syntaxTree = nullptr;
    bool ok = true;
    string errMsg;
    for (TestValue test : tests)
    {
        delete syntaxTree;

        Config config;
        config.color = Config::eFalse;
        stringstream errStream;
        CompilerContext compilerContext(config, errStream);
        syntaxTree = CreateSyntaxTree(errStream, compilerContext, test.input, errMsg);
        if (syntaxTree == nullptr)
        {
            ok = false;
            failMsg = "Creating syntax tree failed: " + errMsg;
            break;
        }

        Expression* expression = nullptr;
        for (const ConstantDeclaration* constDecl : syntaxTree->modules[0]->constantDeclarations)
        {
            if (constDecl->name == "test")
            {
                expression = constDecl->assignmentExpression->right;
                break;
            }
        }

        if (expression == nullptr)
        {
            ok = false;
            failMsg = "Could not find test constant";
            break;
        }

        StartEndTokenFinder finder;
        expression->Accept(&finder);

        const Token* start = finder.start;
        unsigned expectedStartLine = test.startLineOffset;
        unsigned expectedStartColumn = test.startColumnOffset;
        if (expectedStartLine != start->line || expectedStartColumn != start->column)
        {
            ok = false;
            stringstream ss;
            ss
                << "Unexpected token start position: Expected "
                << expectedStartLine << ":" << expectedStartColumn
                << ", actual: "
                << start->line << ":" << start->column;
            failMsg = ss.str();
            break;
        }

        const Token* end = finder.end;
        unsigned expectedEndLine = test.endLineOffset;
        unsigned expectedEndColumn = test.endColumnOffset;
        unsigned actualEndColumn = end->column + end->value.GetSize() - 1;
        if (expectedEndLine != end->line || expectedEndColumn != actualEndColumn)
        {
            ok = false;
            stringstream ss;
            ss
                << "Unexpected token end position: Expected "
                << expectedEndLine << ":" << expectedEndColumn
                << ", actual: "
                << end->line << ":" << actualEndColumn;
            failMsg = ss.str();
            break;
        }
    }

    delete syntaxTree;

    return ok;
}
