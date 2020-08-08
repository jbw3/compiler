#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include "Token.h"
#include <cstring>
#include <ostream>

class ErrorLogger
{
public:
    const char* const WARNING_TAG = "warning";
    const char* const ERROR_TAG = "error";
    const char* const INTERNAL_ERROR_TAG = "internal error";

    ErrorLogger(std::ostream* os) :
        os(os)
    {
    }

    template<typename... Ts>
    void LogWarning(const char* format, Ts... args)
    {
        LogMessage(WARNING_TAG, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const std::string& filename, unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        LogSourceMessage(WARNING_TAG, filename, line, column, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const Token& token, const char* format, Ts... args)
    {
        LogSourceMessage(WARNING_TAG, token.GetFilename(), token.GetLine(), token.GetColumn(), format, args...);
    }

    template<typename... Ts>
    void LogError(const char* format, Ts... args)
    {
        LogMessage(ERROR_TAG, format, args...);
    }

    template<typename... Ts>
    void LogError(const std::string& filename, unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        LogSourceMessage(ERROR_TAG, filename, line, column, format, args...);
    }

    template<typename... Ts>
    void LogError(const Token& token, const char* format, Ts... args)
    {
        LogSourceMessage(ERROR_TAG, token.GetFilename(), token.GetLine(), token.GetColumn(), format, args...);
    }

    template<typename... Ts>
    void LogInternalError(const char* format, Ts... args)
    {
        LogMessage(INTERNAL_ERROR_TAG, format, args...);
    }

    template<typename... Ts>
    void LogInternalError(const Token& token, const char* format, Ts... args)
    {
        LogSourceMessage(INTERNAL_ERROR_TAG, token.GetFilename(), token.GetLine(), token.GetColumn(), format, args...);
    }

private:
    std::ostream* os;

    void Write(const char* format);

    void WriteHeader(const char* tag, const std::string& filename, unsigned long line, unsigned long column);

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
        *os << tag << ": ";
        Write(format, args...);
        *os << '\n';
    }

    template<typename... Ts>
    void LogSourceMessage(const char* tag, const std::string& filename, unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        WriteHeader(tag, filename, line, column);
        Write(format, args...);
        *os << '\n';
    }
};

#endif // ERROR_LOGGER_H_
