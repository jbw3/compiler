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
