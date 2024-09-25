#ifndef TEST_CLASS_H_
#define TEST_CLASS_H_

#include <ostream>
#include <vector>

#define ADD_TEST(test) AddTest(#test, test)

class TestClass
{
public:
    typedef bool (*TestFunc)(std::string& failMsg);

    TestClass(const std::string& name, std::ostream& results);

    bool Run();

    const std::string& GetName() const
    {
        return name;
    }

protected:
    void AddTest(const std::string& testName, TestFunc test);

private:
    struct TestData
    {
        std::string name;
        TestFunc test;
        std::string failMsg;
        bool passed;
    };

    std::string name;
    std::ostream& results;
    std::vector<TestData> tests;
};

#endif // TEST_CLASS_H_
