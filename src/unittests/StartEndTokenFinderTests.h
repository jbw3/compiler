#ifndef START_END_TOKEN_FINDER_TESTS_H_
#define START_END_TOKEN_FINDER_TESTS_H_

#include "TestClass.h"
#include <sstream>

namespace SyntaxTree
{
class Modules;
}
class CompilerContext;

class StartEndTokenFinderTests : public TestClass
{
public:
    StartEndTokenFinderTests(std::ostream& results);

private:
    static SyntaxTree::Modules* CreateSyntaxTree(
        std::stringstream& errStream,
        CompilerContext& compilerContext,
        const std::string& input,
        std::string& failMsg
    );

    static bool Test(std::string& failMsg);
};

#endif // START_END_TOKEN_FINDER_TESTS_H_
