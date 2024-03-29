#ifndef C_HEADER_PRINTER_H_
#define C_HEADER_PRINTER_H_

#include "Config.h"
#include "SyntaxTree.h"
#include <unordered_set>

class ErrorLogger;

class CHeaderPrinter
{
public:
    CHeaderPrinter(ErrorLogger& logger);

    bool Print(const Config& config, const SyntaxTree::Modules* modules);

private:
    ErrorLogger& logger;
    std::unordered_set<ROString> arrayTypeNames;

    bool WriteFile(const std::string& tempFilename, const std::string& outFilename, const SyntaxTree::Modules* modules);

    std::string GetOutFilename(const Config& config);

    std::string GetFilenameMacro(const std::string& outFilename);

    bool PrintCType(std::ostream& os, const TypeInfo* type, ROString varName);

    bool PrintCFunctionType(std::ostream& os, const TypeInfo* type, ROString varName, unsigned pointerCount);

    bool PrintArrayStruct(std::ostream& os, const TypeInfo* arrayType);
};

#endif // C_HEADER_PRINTER_H_
