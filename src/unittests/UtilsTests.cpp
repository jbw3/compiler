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
        make_tuple("1_234", true, true, 1234),
        make_tuple("1_", true, true, 1),
        make_tuple("1_1", true, true, 11),
        make_tuple("0b0", true, true, 0b0),
        make_tuple("0b1", true, true, 0b1),
        make_tuple("0b101", true, true, 0b101),
        make_tuple("0b_1_0_1_0", true, true, 0b1010),
        make_tuple("0b_____010_____", true, true, 0b010),
        make_tuple("0o0", true, true, 00),
        make_tuple("0o4", true, true, 04),
        make_tuple("0o7", true, true, 07),
        make_tuple("0o23", true, true, 023),
        make_tuple("0o1702", true, true, 01702),
        make_tuple("0o716_012", true, true, 0716012),
        make_tuple("0x4", true, true, 0x4),
        make_tuple("0xa", true, true, 0xa),
        make_tuple("0xE", true, true, 0xE),
        make_tuple("0x10Ea5C", true, true, 0x10Ea5C),
        make_tuple("0x10ea_5C7a_39c8_", true, true, 0x10ea5C7a39c8),

        make_tuple("0b", true, false, 0),
        make_tuple("0o", true, false, 0),
        make_tuple("0x", true, false, 0),
        make_tuple("0b_", true, false, 0),
        make_tuple("0o__", true, false, 0),
        make_tuple("0x___", true, false, 0),

        make_tuple("", false, false, 0),
        make_tuple("abc", false, false, 0),
        make_tuple("23a", false, false, 0),
        make_tuple("0b120", false, false, 0),
        make_tuple("0B10", false, false, 0),
        make_tuple("0B", false, false, 0),
        make_tuple("0o138", false, false, 0),
        make_tuple("0O34", false, false, 0),
        make_tuple("0O", false, false, 0),
        make_tuple("0x23G4", false, false, 0),
        make_tuple("0X19A", false, false, 0),
        make_tuple("0X", false, false, 0),
        make_tuple("_", false, false, 0),
        make_tuple("___", false, false, 0),
        make_tuple("0_b", false, false, 0),
        make_tuple("0_o", false, false, 0),
        make_tuple("0_X", false, false, 0),
        make_tuple("_0b", false, false, 0),
        make_tuple("_0o", false, false, 0),
        make_tuple("_0X", false, false, 0),
        make_tuple("_1", false, false, 0),
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
