#include "ROStringTests.h"
#include "ROString.h"
#include <sstream>

#define TEST_EQUAL(a, b, expected) \
    do \
    { \
        if ((a == b) != expected) \
        { \
            failMsg = expected ? #a " should be equal to " #b : #a " should not be equal to " #b; \
            return false; \
        } \
    } while (false)

#define TEST_NOT_EQUAL(a, b, expected) \
    do \
    { \
        if ((a != b) != expected) \
        { \
            failMsg = expected ? #a " should not be equal to " #b : #a " should be equal to " #b; \
            return false; \
        } \
    } while (false)

using namespace std;

ROStringTests::ROStringTests(ostream& results) :
    TestClass("ROString", results)
{
    ADD_TEST(TestEquality);
    ADD_TEST(TestOStream);
    ADD_TEST(TestHash);
}

bool ROStringTests::TestEquality(string& failMsg)
{
    const char* testStr = "test1test2test123";

    ROString s1(testStr, 5);
    ROString s2(testStr + 5, 5);
    ROString s3(testStr + 10, 7);
    ROString s4(testStr + 10, 5);

    TEST_EQUAL(s1, s2, false);
    TEST_EQUAL(s1, s3, false);
    TEST_EQUAL(s1, s4, true);
    TEST_EQUAL(s1, s1, true);

    TEST_NOT_EQUAL(s1, s2, true);
    TEST_NOT_EQUAL(s1, s3, true);
    TEST_NOT_EQUAL(s1, s4, false);
    TEST_NOT_EQUAL(s1, s1, false);

    return true;
}

bool ROStringTests::TestOStream(string& failMsg)
{
    stringstream ss;

    vector<const char*> tests =
    {
        "",
        "Hello!",
        "test",
        "ABCDEFabcdef0123456789!@#[]\"",
    };

    for (const char* test : tests)
    {
        ROString roStr(test, strlen(test));
        string stdStr(test);

        ss.str("");
        ss << roStr;

        if (ss.str() != stdStr)
        {
            failMsg = "Expected: \""s + stdStr + "\", Actual: \""s + ss.str() + "\""s;
            return false;
        }
    }

    return true;
}

bool ROStringTests::TestHash(string& failMsg)
{
    const char* testStr = "testing123testing123";

    ROString s1(testStr, 10);
    ROString s2(testStr + 10, 10);

    hash<ROString> hasher;
    size_t hash1 = hasher(s1);
    size_t hash2 = hasher(s2);
    if (hash1 != hash2)
    {
        failMsg = "Hashes are not equal";
        return false;
    }

    return true;
}
