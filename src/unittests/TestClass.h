#ifndef TEST_CLASS_H_
#define TEST_CLASS_H_

#include <ostream>
#include <vector>

class TestClass
{
public:
    typedef bool (*TestFunc)();

    TestClass(std::ostream& results);

    bool Run();

protected:
    void AddTest(TestFunc test);

private:
    std::vector<TestFunc> tests;
    std::ostream& results;
};

#endif // TEST_CLASS_H_
