#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include <cstring>
#include <ostream>

class ErrorLogger
{
public:
    ErrorLogger(std::ostream* os) :
        os(os)
    {
    }

    template<typename... Ts>
    void LogWarning(const char* format, Ts... args)
    {
        LogMessage("Warning", format, args...);
    }

    template<typename... Ts>
    void LogError(const char* format, Ts... args)
    {
        LogMessage("Error", format, args...);
    }

private:
    std::ostream* os;

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
        *os << tag << ": ";

        Write(format, args...);
        *os << '\n';
    }
};

#endif // ERROR_LOGGER_H_
