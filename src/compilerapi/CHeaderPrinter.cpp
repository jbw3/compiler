#include "CHeaderPrinter.h"
#include "keywords.h"
#include <fstream>
#include <iostream>
#include <sstream>

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
               "#include <stddef.h>\n"
               "#include <stdint.h>\n\n"
               "#ifdef __cplusplus\n"
               "extern \"C\"\n"
               "{\n"
               "#endif\n\n"
               "struct StrData\n"
               "{\n"
               "    size_t Size;\n"
               "    char Data[0];\n"
               "};\n\n"
               "typedef const struct StrData* str;\n\n";

    string cType;

    // print structs
    for (const StructDefinition* structDef : module->GetStructDefinitions())
    {
        outFile << "struct " << structDef->GetName() << "\n{\n";

        for (const MemberDefinition* member : structDef->GetMembers())
        {
            const TypeInfo* memberType = TypeInfo::GetType(member->GetTypeName());
            if (!GetCType(memberType, cType))
            {
                return false;
            }

            outFile << "    " << cType << " " << member->GetName() << ";\n";
        }

        outFile << "};\n\n";
    }

    // print function declarations
    for (const FunctionDefinition* function : module->GetFunctionDefinitions())
    {
        const FunctionDeclaration* declaration = function->GetDeclaration();

        if (!GetCType(declaration->GetReturnType(), cType))
        {
            return false;
        }

        outFile << cType << " " << declaration->GetName() << "(";

        // print function parameters
        const Parameters& params = declaration->GetParameters();
        size_t numParams = params.size();
        for (size_t i = 0; i < numParams; ++i)
        {
            const Parameter* param = params[i];
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

bool CHeaderPrinter::GetCType(const TypeInfo* type, string& cType)
{
    if (type->IsSameAs(*TypeInfo::UnitType))
    {
        cType = "void";
        return true;
    }
    else if (type->IsBool())
    {
        cType = "bool";
        return true;
    }
    else if (type->IsInt())
    {
        const string shortName = type->GetShortName();

        if (shortName == INT_SIZE_KEYWORD)
        {
            cType = "intptr_t";
        }
        else if (shortName == UINT_SIZE_KEYWORD)
        {
            cType = "uintptr_t";
        }
        else
        {
            stringstream ss;
            if (type->GetSign() == TypeInfo::eUnsigned)
            {
                ss << 'u';
            }
            ss << "int" << type->GetNumBits() << "_t";

            cType = ss.str();
        }

        return true;
    }
    else if (type->IsSameAs(*TypeInfo::GetStringPointerType()))
    {
        cType = "str";
        return true;
    }
    else if (type->IsAggregate())
    {
        cType = type->GetShortName();
        return true;
    }
    else
    {
        cerr << "Unsupported type\n";
        cType = "";
        return false;
    }
}
