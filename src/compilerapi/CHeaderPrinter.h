#ifndef C_HEADER_PRINTER_H_
#define C_HEADER_PRINTER_H_

#include "Config.h"
#include "SyntaxTree.h"
#include <unordered_set>

class CompilerContext;
class ErrorLogger;

class CHeaderPrinter
{
public:
    static std::string GetFilenameMacro(const std::string& outFilename);

    CHeaderPrinter(CompilerContext& compilerContext);

    bool Print(const SyntaxTree::Modules* modules);

private:
    ErrorLogger& logger;
    CompilerContext& compilerContext;
    std::unordered_set<ROString> arrayTypeNames;

    bool WriteFile(const std::string& tempFilename, const std::string& outFilename, const SyntaxTree::Modules* modules);

    std::string GetOutFilename(const Config& config);

    bool PrintCType(std::ostream& os, const TypeInfo* type, ROString varName);

    bool PrintCFunctionType(std::ostream& os, const TypeInfo* type, ROString varName, unsigned pointerCount);

    bool PrintArrayStruct(std::ostream& os, const TypeInfo* arrayType);
};

#endif // C_HEADER_PRINTER_H_
