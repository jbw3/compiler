#include "LexicalAnalyzerTests.h"
#include "LexicalAnalyzer.h"
#include <sstream>

using namespace std;

bool LexicalAnalyzerTests::Run()
{
    bool passed = true;

    passed &= TestValidInputs();

    return passed;
}

bool LexicalAnalyzerTests::TestValidInputs()
{
    stringstream ss;
    vector<Token> tokens;

    LexicalAnalyzer analyzer;

    ss.str("fun test() 1 + 2");
    bool ok = analyzer.Process(ss, tokens);
    if (!ok)
    {
        return false;
    }

    return true;
}
