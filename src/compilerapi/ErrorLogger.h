#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include <ostream>

class ErrorLogger
{
public:
    ErrorLogger(std::ostream* os) :
        os(os)
    {
    }

    template<typename... Ts>
    void LogWarning(Ts... args)
    {
        LogMessage("Warning", args...);
    }

    template<typename... Ts>
    void LogError(Ts... args)
    {
        LogMessage("Error", args...);
    }

private:
    std::ostream* os;

    template<typename T>
    void Write(T arg)
    {
        *os << arg;
    }

    template<typename T, typename... Ts>
    void Write(T arg, Ts... args)
    {
        Write(arg);
        Write(args...);
    }

    template<typename... Ts>
    void LogMessage(const char* tag, Ts... args)
    {
        *os << tag << ": ";
        Write(args...);
        *os << '\n';
    }
};

#endif // ERROR_LOGGER_H_
