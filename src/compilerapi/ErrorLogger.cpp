#include "ErrorLogger.h"

void ErrorLogger::Write(const char* format)
{
    *os << format;
}
