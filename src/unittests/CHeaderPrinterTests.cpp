#include "CHeaderPrinterTests.h"
#include "Compiler.h"
#include "CHeaderPrinter.h"
#include "SyntaxTree.h"
#include <filesystem>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace std;

CHeaderPrinterTests::CHeaderPrinterTests(ostream& results) :
    TestClass("CHeaderPrinter", results)
{
    ADD_TEST(TestGetFilenameMacro);
}

bool CHeaderPrinterTests::TestGetFilenameMacro(string& failMsg)
{
    vector<tuple<const char*, const char*>> tests =
    {
        make_tuple("abc.h", "ABC_H_"),
        make_tuple("abc_123.h", "ABC_123_H_"),
        make_tuple("a-b.h", "A_B_H_"),
        make_tuple("123.h", "X_123_H_"),
    };

    bool ok = true;
    for (tuple<const char*, const char*> test : tests)
    {
        const char* input = get<0>(test);
        const char* expected = get<1>(test);

        string actual = CHeaderPrinter::GetFilenameMacro(input);
        if (expected != actual)
        {
            failMsg = "Expected: "s + expected + ", actual: "s + actual;
            ok = false;
        }
    }

    return ok;
}
