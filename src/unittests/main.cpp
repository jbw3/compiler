#include "LexicalAnalyzerTests.h"
#include <iostream>

using namespace std;

int main()
{
    LexicalAnalyzerTests lexicalAnalyzerTests;
    bool passed = lexicalAnalyzerTests.Run();

    return passed ? 0 : 1;
}
