#ifndef C_HEADER_PRINTER_H_
#define C_HEADER_PRINTER_H_

#include "SyntaxTree.h"

class CHeaderPrinter
{
public:
    bool Print(const SyntaxTree::ModuleDefinition* module);

private:
    bool GetCType(SyntaxTree::EType type, std::string& cType);
};

#endif // C_HEADER_PRINTER_H_
