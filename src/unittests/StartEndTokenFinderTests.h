#ifndef START_END_TOKEN_FINDER_TESTS_H_
#define START_END_TOKEN_FINDER_TESTS_H_

#include "TestClass.h"

namespace SyntaxTree
{
class Modules;
}

class StartEndTokenFinderTests : public TestClass
{
public:
    StartEndTokenFinderTests(std::ostream& results);

private:
    static SyntaxTree::Modules* CreateSyntaxTree(const std::string& input, std::string& failMsg);

    static bool Test(std::string& failMsg);
};

#endif // START_END_TOKEN_FINDER_TESTS_H_
