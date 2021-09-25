#include "CHeaderPrinter.h"
#include "keywords.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

bool CHeaderPrinter::Print(const Config& config, const Modules* modules)
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
    for (const StructDefinition* structDef : modules->orderedStructDefinitions)
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
            if (!PrintCType(outFile, memberType, member->name))
            {
                return false;
            }

            outFile << ";\n";
        }

        outFile << "};\n\n";
    }

    for (const ModuleDefinition* module : modules->modules)
    {
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

            if (!PrintCType(outFile, returnType, ""))
            {
                return false;
            }

            outFile << " " << declaration->name << "(";

            // print function parameters
            size_t numParams = params.size();
            for (size_t i = 0; i < numParams; ++i)
            {
                const Parameter* param = params[i];
                if (!PrintCType(outFile, param->type, param->name))
                {
                    return false;
                }

                if (i < numParams - 1)
                {
                    outFile << ", ";
                }
            }

            outFile << ");\n\n";
        }
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
        // if there is no output filename specified, just pick the first input filename
        string filename;
        if (config.inFilenames.empty())
        {
            filename = "-";
        }
        else
        {
            filename = config.inFilenames[0];
        }

        size_t idx = filename.rfind('.');
        outFilename = filename.substr(0, idx);
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

void PrintArrayName(ostream& os, const TypeInfo* arrayType)
{
    os << "struct array_";

    const TypeInfo* innerType = arrayType->GetInnerType();
    while (innerType->IsArray() || innerType->IsPointer())
    {
        if (innerType->IsArray())
        {
            os << "array_";
        }
        else if (innerType->IsPointer())
        {
            os << "ptr_";
        }
        innerType = innerType->GetInnerType();
    }

    os << innerType->GetShortName();
}

void printVarName(ostream& os, const string& varName)
{
    if (!varName.empty())
    {
        os << ' ' << varName;
    }
}

bool CHeaderPrinter::PrintCType(ostream& os, const TypeInfo* type, const string& varName)
{
    if (type->IsSameAs(*TypeInfo::UnitType))
    {
        os << "void";
        printVarName(os, varName);
        return true;
    }
    else if (type->IsBool())
    {
        os << "bool";
        printVarName(os, varName);
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

        printVarName(os, varName);
        return true;
    }
    else if (type->IsPointer())
    {
        unsigned pointerCount = 1;
        const TypeInfo* innerType = type->GetInnerType();
        while (innerType->IsPointer())
        {
            ++pointerCount;
            innerType = innerType->GetInnerType();
        }

        if (innerType->IsFunction())
        {
            if (!PrintCFunctionType(os, innerType, varName, pointerCount))
            {
                return false;
            }
        }
        else
        {
            if (!PrintCType(os, innerType, ""))
            {
                return false;
            }

            for (unsigned i = 0; i < pointerCount; ++i)
            {
                os << '*';
            }
            printVarName(os, varName);
        }

        return true;
    }
    else if (type->IsSameAs(*TypeInfo::GetStringType()))
    {
        os << "struct str";
        printVarName(os, varName);
        return true;
    }
    else if (type->IsArray())
    {
        PrintArrayName(os, type);
        printVarName(os, varName);
        return true;
    }
    else if (type->IsAggregate())
    {
        os << "struct " << type->GetShortName();
        printVarName(os, varName);
        return true;
    }
    else if (type->IsFunction())
    {
        return PrintCFunctionType(os, type, varName, 0);
    }
    else
    {
        // TODO: use error logger
        cerr << "Unsupported type\n";
        return false;
    }
}

bool CHeaderPrinter::PrintCFunctionType(std::ostream& os, const TypeInfo* type, const std::string& varName, unsigned pointerCount)
{
    if (!PrintCType(os, type->GetReturnType(), ""))
    {
        return false;
    }
    os << " (*";
    for (unsigned i = 0; i < pointerCount; ++i)
    {
        os << '*';
    }
    os << varName << ")(";

    size_t numParams = type->GetParamTypes().size();
    for (size_t i = 0; i < numParams; ++i)
    {
        if (!PrintCType(os, type->GetParamTypes()[i], type->GetParamNames()[i]))
        {
            return false;
        }

        if (i < numParams - 1)
        {
            os << ", ";
        }
    }

    os << ")";

    return true;
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

        PrintArrayName(os, arrayType);

        os << "\n"
              "{\n"
              "    size_t Size;\n"
              "    ";
        if (!PrintCType(os, innerType, ""/*TODO: does this work for arrays of function pointers?*/))
        {
            return false;
        }
        os << "* Data;\n};\n\n";

        arrayTypeNames.insert(typeName);
    }

    return true;
}
