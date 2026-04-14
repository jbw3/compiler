#include "StartEndTokenFinderTests.h"
#include "CompilerContext.h"
#include "LexicalAnalyzer.h"
#include "StartEndTokenFinder.h"
#include "SyntaxAnalyzer.h"
#include "SyntaxTree.h"
#include <sstream>

using namespace std;
using namespace SyntaxTree;

StartEndTokenFinderTests::StartEndTokenFinderTests(ostream &results) :
    TestClass("StartEndTokenFinder", results)
{
    ADD_TEST(Test);
}

Modules* StartEndTokenFinderTests::CreateSyntaxTree(const std::string& input, std::string& failMsg)
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
        {" - abc", 1, 2, 1, 6},

        // binary
        {"x + 2", 1, 1, 1, 5},
        {" xyz / abc - 1_000", 1, 2, 1, 18},
        {"xyz[17]", 1, 1, 1, 7},

        // function type expression
        {"fun (a i32, b bool)", 1, 1, 1, 19},
        {"fun (a i32, b bool) str", 1, 1, 1, 23},
        {"fun (a i32,\nb bool) str", 1, 1, 2, 11},

        // numeric
        {"0x10", 1, 1, 1, 4},

        // float
        {"1.234", 1, 1, 1, 5},
        {"0.99e-30", 1, 1, 1, 8},

        // bool
        {"true", 1, 1, 1, 4},
        {"false", 1, 1, 1, 5},

        // string
        {"\"abc\"", 1, 1, 1, 5},
        {"\"123\\n456\"", 1, 1, 1, 10},

        // built-in identifier
        {"@pi", 1, 1, 1, 3},

        // array size/value
        {"[3; 17.5]", 1, 1, 1, 9},

        // array multi-value
        {"[1, 2, 300]", 1, 1, 1, 11},
        {"[\n1.2,\n@pi,\n77e3,\n]", 1, 1, 5, 1},
    };

    Modules* syntaxTree = nullptr;
    bool ok = true;
    string errMsg;
    for (TestValue test : tests)
    {
        string input = "const x = " + test.input + ";";

        delete syntaxTree;
        syntaxTree = CreateSyntaxTree(input, errMsg);
        if (syntaxTree == nullptr)
        {
            ok = false;
            failMsg = "Creating syntax tree failed: " + errMsg;
            break;
        }

        Expression* expression = syntaxTree->modules[0]->constantDeclarations[0]->assignmentExpression->right;

        StartEndTokenFinder finder;
        expression->Accept(&finder);

        const Token* start = finder.start;
        unsigned expectedStartLine = test.startLineOffset;
        unsigned expectedStartColumn = test.startColumnOffset;
        if (expectedStartLine == 1)
        {
            expectedStartColumn += 10;
        }
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
        if (expectedEndLine == 1)
        {
            expectedEndColumn += 10;
        }
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
