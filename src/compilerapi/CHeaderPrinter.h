#ifndef C_HEADER_PRINTER_H_
#define C_HEADER_PRINTER_H_

#include "Config.h"
#include "SyntaxTree.h"

class CHeaderPrinter
{
public:
    bool Print(const Config& config, const SyntaxTree::ModuleDefinition* module);

private:
    std::string GetOutFilename(const Config& config);

    std::string GetFilenameMacro(const std::string& outFilename);

    bool GetCType(const TypeInfo* type, std::string& cType);
};

#endif // C_HEADER_PRINTER_H_
