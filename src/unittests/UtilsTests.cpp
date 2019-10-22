#include "UtilsTests.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

UtilsTests::UtilsTests()
{
    AddTest(TestNumberConversion);
}

bool UtilsTests::TestNumberConversion()
{
    vector<tuple<string, bool, bool, int64_t>> tests =
    {
        make_tuple("0", true, true, 0),
        make_tuple("5", true, true, 5),
        make_tuple("10", true, true, 10),
        make_tuple("123", true, true, 123),
        make_tuple("1234567890", true, true, 1234567890),
        make_tuple("0b0", true, true, 0b0),
        make_tuple("0b1", true, true, 0b1),
        make_tuple("0B101", true, true, 0B101),
        make_tuple("0o0", true, true, 00),
        make_tuple("0O4", true, true, 04),
        make_tuple("0o7", true, true, 07),
        make_tuple("0O23", true, true, 023),
        make_tuple("0o1702", true, true, 01702),
        make_tuple("0x4", true, true, 0x4),
        make_tuple("0Xa", true, true, 0Xa),
        make_tuple("0xE", true, true, 0xE),
        make_tuple("0X10Ea5C", true, true, 0X10Ea5C),

        make_tuple("0b", true, false, 0),
        make_tuple("0B", true, false, 0),
        make_tuple("0o", true, false, 0),
        make_tuple("0O", true, false, 0),
        make_tuple("0x", true, false, 0),
        make_tuple("0X", true, false, 0),

        make_tuple("abc", false, false, 0),
        make_tuple("23a", false, false, 0),
        make_tuple("0b120", false, false, 0),
        make_tuple("0O138", false, false, 0),
        make_tuple("0x23G4", false, false, 0),
    };

    bool ok = true;
    for (tuple<string, bool, bool, int64_t> test : tests)
    {
        string& testStr = get<0>(test);

        int64_t num = -1;
        bool resultPotential = isPotentialNumber(testStr);
        bool resultActual = isNumber(testStr);
        bool resultConversion = stringToInteger(testStr, num);

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

        if (resultConversion != get<2>(test))
        {
            cerr << "'" << testStr << "' was "
                 << (resultConversion ? "incorrectly converted" : "not converted") << '\n';
            ok = false;
        }

        if (num != get<3>(test))
        {
            cerr << "'" << testStr << "' was incorrectly converted to '" << num << "'\n";
            ok = false;
        }

        if (!ok)
        {
            break;
        }
    }

    return ok;
}