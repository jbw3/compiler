#ifndef C_HEADER_PRINTER_H_
#define C_HEADER_PRINTER_H_

#include "Config.h"
#include "SyntaxTree.h"
#include <unordered_set>

class CHeaderPrinter
{
public:
    bool Print(const Config& config, const SyntaxTree::ModuleDefinition* module);

private:
    std::unordered_set<std::string> arrayTypeNames;

    std::string GetOutFilename(const Config& config);

    std::string GetFilenameMacro(const std::string& outFilename);

    bool PrintCType(std::ostream& os, const TypeInfo* type);

    bool PrintArrayStruct(std::ostream& os, const TypeInfo* arrayType);
};

#endif // C_HEADER_PRINTER_H_
