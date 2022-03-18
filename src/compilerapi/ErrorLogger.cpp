#include "CompilerContext.h"
#include "ErrorLogger.h"
#include <iostream>
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

bool isConsole(ostream* os)
{
    bool value = false;

#ifdef _WIN32
    if (os == &cerr)
    {
        value = _isatty(_fileno(stderr));
    }
    else if (os == &cout)
    {
        value = _isatty(_fileno(stdout));
    }
#else
    if (os == &cerr)
    {
        value = isatty(STDERR_FILENO);
    }
    else if (os == &cout)
    {
        value = isatty(STDOUT_FILENO);
    }
#endif

    return value;
}

bool configureConsole([[maybe_unused]] ostream* os)
{
#ifdef _WIN32
    bool ok = false;

    HANDLE handle = INVALID_HANDLE_VALUE;
    if (os == &cerr)
    {
        handle = GetStdHandle(STD_ERROR_HANDLE);
    }
    else if (os == &cout)
    {
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD originalMode = 0;
        if (GetConsoleMode(handle, &originalMode))
        {
            DWORD newErrMode = originalMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(handle, newErrMode))
            {
                ok = true;
            }
        }
    }

    return ok;
#else
    // nothing to do on Linux
    return true;
#endif
}

ErrorLogger::ErrorLogger(CompilerContext& compilerContext, ostream* os, Config::EColor color) :
    compilerContext(compilerContext),
    os(os)
{
    if (color == Config::eAuto)
    {
        printColors = isConsole(os) && configureConsole(os);
    }
    else if (color == Config::eTrue)
    {
        configureConsole(os);
        printColors = true;
    }
    else // if (color == Config::eFalse)
    {
        printColors = false;
    }
}

void ErrorLogger::SetBold()
{
    if (printColors)
    {
        *os << "\x1B[1m";
    }
}

void ErrorLogger::SetColor(const char* tag)
{
    if (printColors)
    {
        if (strcmp(tag, NOTE_TAG) == 0)
        {
            *os << "\x1B[36m";
        }
        else if (strcmp(tag, WARNING_TAG) == 0)
        {
            *os << "\x1B[33m";
        }
        else if (strcmp(tag, ERROR_TAG) == 0 || strcmp(tag, INTERNAL_ERROR_TAG) == 0)
        {
            *os << "\x1B[31m";
        }
    }
}

void ErrorLogger::ResetFormat()
{
    if (printColors)
    {
        *os << "\x1B[0m";
    }
}

void ErrorLogger::WriteHeader(const char* tag, unsigned filenameId, unsigned line, unsigned column)
{
    string filename = "";
    if (filenameId < compilerContext.GetFileIdCount())
    {
        filename = compilerContext.GetFilename(filenameId);
    }

    SetBold();

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

    if (!filename.empty() || line > 0 || column > 0)
    {
        *os << " ";
    }

    SetColor(tag);
    *os << tag << ':';
    ResetFormat();
    *os << ' ';
}

void ErrorLogger::WriteSourceLine(const char* tag, unsigned filenameId, unsigned startLine, unsigned startColumn, unsigned endLine, unsigned endColumn)
{
    if (filenameId >= compilerContext.GetFileIdCount())
    {
        return;
    }

    CharBuffer buff = compilerContext.GetFileBuffer(filenameId);
    size_t buffSize = buff.size;
    const char* chars = buff.ptr;

    // find the line
    size_t idx = 0;
    unsigned l = 1;
    while (l < startLine)
    {
        while (idx < buffSize && chars[idx] != '\n')
        {
            ++idx;
        }

        ++idx;
        ++l;
    }

    while (l <= endLine)
    {
        // write the line
        bool foundNonWhitespace = false;
        unsigned lineStart = 1;
        unsigned lineEnd = 1;
        while (idx < buffSize && chars[idx] != '\n')
        {
            char ch = chars[idx];
            *os << ch;

            if (!foundNonWhitespace)
            {
                if (isspace(ch))
                {
                    ++lineStart;
                }
                else
                {
                    foundNonWhitespace = true;
                }
            }

            ++idx;
            ++lineEnd;
        }
        ++idx;
        *os << '\n';

        // calculate where the underline should start and end
        unsigned underlineStartColumn = 0;
        unsigned underlineEndColumn = 0;
        if (l == startLine)
        {
            underlineStartColumn = startColumn;
            underlineEndColumn = lineEnd;
        }
        else if (l == endLine)
        {
            underlineStartColumn = lineStart;
            underlineEndColumn = endColumn;
        }
        else
        {
            underlineStartColumn = lineStart;
            underlineEndColumn = lineEnd;
        }

        // underline token
        unsigned c = 1;
        for (; c < underlineStartColumn; ++c)
        {
            *os << ' ';
        }
        SetBold();
        SetColor(tag);
        for (; c < underlineEndColumn; ++c)
        {
            *os << '~';
        }
        ResetFormat();

        *os << '\n';

        ++l;
    }
}

void ErrorLogger::Write(const char* format)
{
    *os << format;
}
