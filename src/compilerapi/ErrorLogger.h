#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#include "Config.h"
#include "Token.h"
#include <cstring>
#include <ostream>

class CompilerContext;

class ErrorLogger
{
public:
    const char* const NOTE_TAG = "note";
    const char* const WARNING_TAG = "warning";
    const char* const ERROR_TAG = "error";
    const char* const INTERNAL_ERROR_TAG = "internal error";

    ErrorLogger(CompilerContext& compilerContext, std::ostream* os, Config::EColor color);

    template<typename... Ts>
    void LogNote(const char* format, Ts... args)
    {
        LogMessage(NOTE_TAG, format, args...);
    }

    template<typename... Ts>
    void LogNote(unsigned filenameId, unsigned line, unsigned column, const char* format, Ts... args)
    {
        LogSourceMessage(NOTE_TAG, filenameId, line, column, line, column + 1, format, args...);
    }

    template<typename... Ts>
    void LogNote(const Token& token, const char* format, Ts... args)
    {
        unsigned width = strlen(token.value);
        LogSourceMessage(NOTE_TAG, token.filenameId, token.line, token.column, token.line, token.column + width, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const char* format, Ts... args)
    {
        LogMessage(WARNING_TAG, format, args...);
    }

    template<typename... Ts>
    void LogWarning(unsigned filenameId, unsigned line, unsigned column, const char* format, Ts... args)
    {
        LogSourceMessage(WARNING_TAG, filenameId, line, column, line, column + 1, format, args...);
    }

    template<typename... Ts>
    void LogWarning(const Token& token, const char* format, Ts... args)
    {
        unsigned width = strlen(token.value);
        LogSourceMessage(WARNING_TAG, token.filenameId, token.line, token.column, token.line, token.column + width, format, args...);
    }

    template<typename... Ts>
    void LogError(const char* format, Ts... args)
    {
        LogMessage(ERROR_TAG, format, args...);
    }

    template<typename... Ts>
    void LogError(unsigned filenameId, unsigned line, unsigned column, const char* format, Ts... args)
    {
        LogSourceMessage(ERROR_TAG, filenameId, line, column, line, column + 1, format, args...);
    }

    template<typename... Ts>
    void LogError(const Token& token, const char* format, Ts... args)
    {
        unsigned width = strlen(token.value);
        LogSourceMessage(ERROR_TAG, token.filenameId, token.line, token.column, token.line, token.column + width, format, args...);
    }

    template<typename... Ts>
    void LogError(const Token& startToken, const Token& endToken, const char* format, Ts... args)
    {
        unsigned endTokenWidth = strlen(endToken.value);
        LogSourceMessage(ERROR_TAG, startToken.filenameId, startToken.line, startToken.column, endToken.line, endToken.column + endTokenWidth, format, args...);
    }

    template<typename... Ts>
    void LogInternalError(const char* format, Ts... args)
    {
        LogMessage(INTERNAL_ERROR_TAG, format, args...);
    }

private:
    CompilerContext& compilerContext;
    std::ostream* os;
    bool printColors;

    void SetBold();

    void SetColor(const char* tag);

    void ResetFormat();

    void WriteHeader(const char* tag, unsigned filenameId, unsigned line, unsigned column);

    void WriteSourceLine(const char* tag, unsigned filenameId, unsigned startLine, unsigned startColumn, unsigned endLine, unsigned endColumn);

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
        WriteHeader(tag, static_cast<unsigned>(-1), 0, 0);
        Write(format, args...);
        *os << '\n';
    }

    template<typename... Ts>
    void LogSourceMessage(const char* tag, unsigned filenameId, unsigned startLine, unsigned startColumn, unsigned endLine, unsigned endColumn, const char* format, Ts... args)
    {
        WriteHeader(tag, filenameId, startLine, startColumn);
        Write(format, args...);
        *os << '\n';
        WriteSourceLine(tag, filenameId, startLine, startColumn, endLine, endColumn);
    }
};

#endif // ERROR_LOGGER_H_
