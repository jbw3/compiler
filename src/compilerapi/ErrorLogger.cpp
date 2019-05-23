#include "ErrorLogger.h"

void ErrorLogger::Write(const char* format)
{
    *os << format;
}

void ErrorLogger::WriteHeader(const char* tag, unsigned long line, unsigned long column)
{
    *os << tag << ":";

    if (line > 0)
    {
        *os << line;

        if (column > 0)
        {
            *os << ":" << column;
        }

        *os << ":";
    }

    *os << " ";
}
