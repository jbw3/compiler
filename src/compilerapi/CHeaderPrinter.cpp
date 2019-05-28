#include "CHeaderPrinter.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

bool CHeaderPrinter::Print(const Config& config, const ModuleDefinition* module)
{
    string outFilename = GetOutFilename(config);

    fstream outFile(outFilename.c_str(), ios_base::out);

    string macro = GetFilenameMacro(outFilename);

    outFile << "#ifndef " << macro << "\n"
               "#define " << macro << "\n\n"
               "#include <stdbool.h>\n"
               "#include <stdint.h>\n\n"
               "#ifdef __cplusplus\n"
               "extern \"C\"\n"
               "{\n"
               "#endif\n\n";

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

    outFile << "#ifdef __cplusplus\n"
               "} /* extern \"C\" */\n"
               "#endif\n\n"
               "#endif /* " << macro << " */\n";

    return true;
}

string CHeaderPrinter::GetOutFilename(const Config& config)
{
    string outFilename;
    if (config.outFilename.empty())
    {
        size_t idx = config.inFilename.rfind('.');
        outFilename = config.inFilename.substr(0, idx);
        outFilename += ".h";
    }
    else
    {
        outFilename = config.outFilename;
    }

    return outFilename;
}

string CHeaderPrinter::GetFilenameMacro(const string& outFilename)
{
    string macro;

    // make sure the first char is valid for an identifier
    if (!outFilename.empty())
    {
        char firstChar = outFilename[0];
        if (!isalpha(firstChar) && firstChar != '_')
        {
            macro += "X_";
        }
    }

    for (char ch : outFilename)
    {
        if (isalnum(ch))
        {
            macro += toupper(ch);
        }
        else
        {
            macro += "_";
        }
    }
    macro += "_";

    return macro;
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
