#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include <cstring>
#include <ostream>

class ErrorLogger
{
public:
    const char* const WARNING_TAG = "Warning";
    const char* const ERROR_TAG = "Error";

    ErrorLogger(std::ostream* os) :
        os(os)
    {
    }

    template<typename... Ts>
    void LogWarning(unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        LogMessage(WARNING_TAG, line, column, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const char* format, Ts... args)
    {
        LogMessage(WARNING_TAG, 0, 0, format, args...);
    }

    template<typename... Ts>
    void LogError(unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        LogMessage(ERROR_TAG, line, column, format, args...);
    }

    template<typename... Ts>
    void LogError(const char* format, Ts... args)
    {
        LogMessage(ERROR_TAG, 0, 0, format, args...);
    }

private:
    std::ostream* os;

    void Write(const char* format);

    void WriteHeader(const char* tag, unsigned long line, unsigned long column);

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
    void LogMessage(const char* tag, unsigned long line, unsigned long column, const char* format, Ts... args)
    {
        WriteHeader(tag, line, column);
        Write(format, args...);
        *os << '\n';
    }
};

#endif // ERROR_LOGGER_H_
