#include "CHeaderPrinter.h"
#include "keywords.h"
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
               "#include <stddef.h>\n"
               "#include <stdint.h>\n\n"
               "#ifdef __cplusplus\n"
               "extern \"C\"\n"
               "{\n"
               "#endif\n\n"
               "struct str\n"
               "{\n"
               "    size_t Size;\n"
               "    char* Data;\n"
               "};\n\n";

    // print structs
    for (const StructDefinition* structDef : module->structDefinitions)
    {
        const TypeInfo* structType = structDef->type;

        // print array structs
        for (const MemberDefinition* member : structDef->members)
        {
            const string& memberName = member->name;
            const MemberInfo* memberInfo = structType->GetMember(memberName);
            const TypeInfo* memberType = memberInfo->GetType();
            if (memberType->IsArray())
            {
                if (!PrintArrayStruct(outFile, memberType))
                {
                    return false;
                }
            }
        }

        outFile << "struct " << structDef->name << "\n{\n";

        for (const MemberDefinition* member : structDef->members)
        {
            const MemberInfo* memberInfo = structType->GetMember(member->name);
            const TypeInfo* memberType = memberInfo->GetType();

            outFile << "    ";
            if (!PrintCType(outFile, memberType))
            {
                return false;
            }

            outFile << " " << member->name << ";\n";
        }

        outFile << "};\n\n";
    }

    // print function declarations
    for (const FunctionDefinition* function : module->functionDefinitions)
    {
        const FunctionDeclaration* declaration = function->declaration;
        const Parameters& params = declaration->parameters;
        const TypeInfo* returnType = declaration->returnType;

        // print array structs
        if (returnType->IsArray())
        {
            if (!PrintArrayStruct(outFile, returnType))
            {
                return false;
            }
        }
        for (const Parameter* param : params)
        {
            const TypeInfo* paramType = param->type;
            if (paramType->IsArray())
            {
                if (!PrintArrayStruct(outFile, paramType))
                {
                    return false;
                }
            }
        }

        if (!PrintCType(outFile, returnType))
        {
            return false;
        }

        outFile << " " << declaration->name << "(";

        // print function parameters
        size_t numParams = params.size();
        for (size_t i = 0; i < numParams; ++i)
        {
            const Parameter* param = params[i];
            if (!PrintCType(outFile, param->type))
            {
                return false;
            }

            outFile << " " << param->name;
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

bool CHeaderPrinter::PrintCType(ostream& os, const TypeInfo* type)
{
    if (type->IsSameAs(*TypeInfo::UnitType))
    {
        os << "void";
        return true;
    }
    else if (type->IsBool())
    {
        os << "bool";
        return true;
    }
    else if (type->IsInt())
    {
        const string shortName = type->GetShortName();

        if (shortName == INT_SIZE_KEYWORD)
        {
            os << "intptr_t";
        }
        else if (shortName == UINT_SIZE_KEYWORD)
        {
            os << "uintptr_t";
        }
        else
        {
            if (type->GetSign() == TypeInfo::eUnsigned)
            {
                os << 'u';
            }
            os << "int" << type->GetNumBits() << "_t";
        }

        return true;
    }
    else if (type->IsPointer())
    {
        if (!PrintCType(os, type->GetInnerType()))
        {
            return false;
        }

        os << "*";
        return true;
    }
    else if (type->IsSameAs(*TypeInfo::GetStringType()))
    {
        os << "struct str";
        return true;
    }
    else if (type->IsArray())
    {
        os << "struct ";
        const TypeInfo* tempType = type;
        while (tempType->IsArray())
        {
            os << "array_";
            tempType = tempType->GetInnerType();
        }
        return PrintCType(os, tempType);
    }
    else if (type->IsAggregate())
    {
        os << "struct " << type->GetShortName();
        return true;
    }
    else
    {
        // TODO: use error logger
        cerr << "Unsupported type\n";
        return false;
    }
}

bool CHeaderPrinter::PrintArrayStruct(ostream& os, const TypeInfo* arrayType)
{
    const string& typeName = arrayType->GetShortName();
    if (arrayTypeNames.find(typeName) == arrayTypeNames.end())
    {
        const TypeInfo* innerType = arrayType->GetInnerType();

        if (innerType->IsArray())
        {
            if (!PrintArrayStruct(os, innerType))
            {
                return false;
            }
        }

        os << "struct array_";

        const TypeInfo* tempType = innerType;
        while (tempType->IsArray())
        {
            os << "array_";
            tempType = tempType->GetInnerType();
        }

        if (!PrintCType(os, tempType))
        {
            return false;
        }

        os << "\n"
              "{\n"
              "    size_t Size;\n"
              "    ";
        if (!PrintCType(os, innerType))
        {
            return false;
        }
        os << "* Data;\n};\n\n";

        arrayTypeNames.insert(typeName);
    }

    return true;
}
