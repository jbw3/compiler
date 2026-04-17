#ifndef C_HEADER_PRINTER_TESTS_H_
#define C_HEADER_PRINTER_TESTS_H_

#include "TestClass.h"

class CHeaderPrinterTests : public TestClass
{
public:
    CHeaderPrinterTests(std::ostream& results);

private:
    static bool TestGetFilenameMacro(std::string& failMsg);

    static bool TestPrinter(std::string& failMsg);
};

#endif // C_HEADER_PRINTER_TESTS_H_
