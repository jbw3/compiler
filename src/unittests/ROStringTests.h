#pragma once

#include "TestClass.h"

class ROStringTests : public TestClass
{
public:
    ROStringTests(std::ostream& results);

private:
    static bool TestEquality(std::string& failMsg);

    static bool TestOStream(std::string& failMsg);

    static bool TestHash(std::string& failMsg);
};
