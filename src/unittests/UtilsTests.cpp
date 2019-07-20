#include "UtilsTests.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

UtilsTests::UtilsTests()
{
    AddTest(TestIsPotentialNumber);
}

bool UtilsTests::TestIsPotentialNumber()
{
    vector<tuple<string, bool, bool>> tests =
    {
        make_tuple("0", true, true),
        make_tuple("5", true, true),
        make_tuple("10", true, true),
        make_tuple("123", true, true),
        make_tuple("1234567890", true, true),
        make_tuple("0b0", true, true),
        make_tuple("0b1", true, true),
        make_tuple("0B101", true, true),
        make_tuple("0o0", true, true),
        make_tuple("0O4", true, true),
        make_tuple("0o7", true, true),
        make_tuple("0O23", true, true),
        make_tuple("0o1702", true, true),
        make_tuple("0x4", true, true),
        make_tuple("0Xa", true, true),
        make_tuple("0xE", true, true),
        make_tuple("0X10Ea5C", true, true),

        make_tuple("0b", true, false),
        make_tuple("0B", true, false),
        make_tuple("0o", true, false),
        make_tuple("0O", true, false),
        make_tuple("0x", true, false),
        make_tuple("0X", true, false),

        make_tuple("abc", false, false),
        make_tuple("23a", false, false),
        make_tuple("0b120", false, false),
        make_tuple("0O138", false, false),
        make_tuple("0x23G4", false, false),
    };

    bool ok = true;
    for (tuple<string, bool, bool> test : tests)
    {
        string& testStr = get<0>(test);

        bool resultPotential = isPotentialNumber(testStr);
        bool resultActual = isNumber(testStr);

        if (resultPotential != get<1>(test))
        {
            cerr << "'" << testStr << "' was incorrectly identified as "
                 << (resultPotential ? "a potential number" : "not a potential number") << '\n';

            ok = false;
        }

        if (resultActual != get<2>(test))
        {
            cerr << "'" << testStr << "' was incorrectly identified as "
                 << (resultPotential ? "a number" : "not a number") << '\n';

            ok = false;
        }

        if (!ok)
        {
            break;
        }
    }

    return ok;
}
