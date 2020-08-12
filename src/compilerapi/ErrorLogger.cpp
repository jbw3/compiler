#include "ErrorLogger.h"
#include <iostream>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

ErrorLogger::ErrorLogger(std::ostream* os) :
    os(os)
{
#ifdef _WIN32
    // TODO: check os and get corresponding handle
    // TODO: call _isatty()
    printColors = false;
    HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD originalMode = 0;
        if (GetConsoleMode(handle, &originalMode))
        {
            DWORD newErrMode = originalMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(handle, newErrMode))
            {
                printColors = true;
            }
        }
    }
#else
    if (os == &cerr)
    {
        printColors = isatty(STDERR_FILENO);
    }
    else if (os == &cout)
    {
        printColors = isatty(STDOUT_FILENO);
    }
    else
    {
        printColors = false;
    }
#endif
}

void ErrorLogger::Write(const char* format)
{
    *os << format;
}

void ErrorLogger::WriteHeader(const char* tag, const std::string& filename, unsigned long line, unsigned long column)
{
    if (printColors)
    {
        *os << "\x1B[1m";
    }

    if (!filename.empty())
    {
        *os << filename << ":";
    }

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

    if (printColors)
    {
        if (strcmp(tag, WARNING_TAG) == 0)
        {
            *os << "\x1B[33m";
        }
        else if (strcmp(tag, ERROR_TAG) == 0 || strcmp(tag, INTERNAL_ERROR_TAG) == 0)
        {
            *os << "\x1B[31m";
        }
        *os << tag << ":\x1B[0m ";
    }
    else
    {
        *os << tag << ": ";
    }
}
