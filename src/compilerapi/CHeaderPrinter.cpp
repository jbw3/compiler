#include <fstream>
#include <iostream>
#include "CHeaderPrinter.h"

using namespace std;
using namespace SyntaxTree;

bool CHeaderPrinter::Print(const ModuleDefinition* module)
{
    string filename = "temp.h";
    fstream outFile(filename.c_str(), ios_base::out);

    outFile << "#include <stdint.h>\n\n";

    // print function declarations
    string cType;
    for (const FunctionDefinition* function : module->GetFunctionDefinitions())
    {
        if (!GetCType(function->GetReturnType(), cType))
        {
            return false;
        }

        outFile << cType << " " << function->GetName() << "(";

        // print function parameters
        size_t numParams = function->GetParameters().size();
        for (size_t i = 0; i < numParams; ++i)
        {
            const VariableDefinition* param = function->GetParameters()[i];
            if (!GetCType(param->GetType(), cType))
            {
                return false;
            }

            outFile << cType << " " << param->GetName();
            if (i < numParams - 1)
            {
                outFile << ", ";
            }
        }

        outFile << ");\n\n";
    }

    return true;
}

bool CHeaderPrinter::GetCType(EType type, string& cType)
{
    switch (type)
    {
        case EType::eUnknown:
            cerr << "Unknown return type\n";
            cType = "";
            return false;
        case EType::eBool:
            cType = "bool";
            return true;
        case EType::eInt32:
            cType = "int32_t";
            return true;
    }
}
