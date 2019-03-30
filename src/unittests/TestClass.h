#ifndef TEST_CLASS_H_
#define TEST_CLASS_H_

#include <vector>

class TestClass
{
public:
    typedef bool (*TestFunc)();

    bool Run();

protected:
    void AddTest(TestFunc test);

private:
    std::vector<TestFunc> tests;
};

#endif // TEST_CLASS_H_
