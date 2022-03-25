#include "ROStringTests.h"
#include "ROString.h"

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
