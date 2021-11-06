#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include "CompilerContext.h"
#include "Config.h"
#include "Token.h"
#include <cstring>
#include <ostream>

class ErrorLogger
{
public:
    const char* const WARNING_TAG = "warning";
    const char* const ERROR_TAG = "error";
    const char* const INTERNAL_ERROR_TAG = "internal error";

    ErrorLogger(CompilerContext& compilerContext, std::ostream* os, Config::EColor color);

    template<typename... Ts>
    void LogWarning(const char* format, Ts... args)
    {
        LogMessage(WARNING_TAG, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const std::string& filename, unsigned line, unsigned column, const char* format, Ts... args)
    {
        LogSourceMessage(WARNING_TAG, filename, line, column, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const Token& token, const char* format, Ts... args)
    {
        const std::string& filename = compilerContext.GetFilename(token.filenameId);
        LogSourceMessage(WARNING_TAG, filename, token.line, token.column, format, args...);
    }

    template<typename... Ts>
    void LogError(const char* format, Ts... args)
    {
        LogMessage(ERROR_TAG, format, args...);
    }

    template<typename... Ts>
    void LogError(const std::string& filename, unsigned line, unsigned column, const char* format, Ts... args)
    {
        LogSourceMessage(ERROR_TAG, filename, line, column, format, args...);
    }

    template<typename... Ts>
    void LogError(const Token& token, const char* format, Ts... args)
    {
        const std::string& filename = compilerContext.GetFilename(token.filenameId);
        LogSourceMessage(ERROR_TAG, filename, token.line, token.column, format, args...);
    }

    template<typename... Ts>
    void LogInternalError(const char* format, Ts... args)
    {
        LogMessage(INTERNAL_ERROR_TAG, format, args...);
    }

    template<typename... Ts>
    void LogInternalError(const Token& token, const char* format, Ts... args)
    {
        const std::string& filename = compilerContext.GetFilename(token.filenameId);
        LogSourceMessage(INTERNAL_ERROR_TAG, filename, token.line, token.column, format, args...);
    }

private:
    CompilerContext& compilerContext;
    std::ostream* os;
    bool printColors;

    void WriteHeader(const char* tag, const std::string& filename, unsigned line, unsigned column);

    void WriteSourceLine(const std::string& filename, unsigned line, unsigned column);

    void Write(const char* format);

    template<typename T, typename... Ts>
    void Write(const char* format, T arg, Ts... args)
    {
        const char* argStr = strchr(format, '{');

        // if there's no arg string, print the format string
        if (argStr == nullptr || argStr[1] != '}')
        {
            Write(format);
        }
        else
        {
            os->write(format, argStr - format);
            *os << arg;

            Write(argStr + 2, args...);
        }
    }

    template<typename... Ts>
    void LogMessage(const char* tag, const char* format, Ts... args)
    {
        WriteHeader(tag, "", 0, 0);
        Write(format, args...);
        *os << '\n';
    }

    template<typename... Ts>
    void LogSourceMessage(const char* tag, const std::string& filename, unsigned line, unsigned column, const char* format, Ts... args)
    {
        WriteHeader(tag, filename, line, column);
        Write(format, args...);
        *os << '\n';
        WriteSourceLine(filename, line, column);
    }
};

#endif // ERROR_LOGGER_H_
