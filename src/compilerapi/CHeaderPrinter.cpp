#include "CHeaderPrinter.h"
#include "CompilerContext.h"
#include "ErrorLogger.h"
#include "keywords.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std;
using namespace SyntaxTree;

CHeaderPrinter::CHeaderPrinter(CompilerContext& compilerContext) :
    logger(compilerContext.logger),
    compilerContext(compilerContext)
{
}

bool CHeaderPrinter::Print(const Config& config, const Modules* modules)
{
    string outFilename = GetOutFilename(config);
    string tempFilename = outFilename + ".0.tmp";

    unsigned i = 0;
    while (fs::exists(tempFilename))
    {
        ++i;
        tempFilename = outFilename + "." + to_string(i) + ".tmp";
    }

    bool ok = WriteFile(tempFilename, outFilename, modules);

    if (ok)
    {
        // rename temp file as output file
        fs::rename(tempFilename, outFilename);
    }
    else
    {
        // delete temp file
        fs::remove(tempFilename);
    }

    return ok;
}

bool CHeaderPrinter::WriteFile(const string& tempFilename, const string& outFilename, const Modules* modules)
{
    fstream outFile(tempFilename.c_str(), ios_base::out);

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

    unordered_map<TypeId, ROString> printedTypeIds;
    for (const ConstantDeclaration* constDecl : modules->orderedGlobalConstants)
    {
        if (constDecl->constantType->IsType())
        {
            unsigned constIdx = constDecl->assignmentExpression->right->GetConstantValueIndex();
            const TypeInfo* type = compilerContext.GetTypeConstantValue(constIdx);
            if (type->IsAggregate())
            {
                TypeId id = type->GetId();
                auto iter = printedTypeIds.find(id);
                if (iter != printedTypeIds.cend())
                {
                    // if we have already printed this struct, make a typedef
                    outFile << "typedef struct " << iter->second << " " << constDecl->name << ";\n\n";
                }
                else
                {
                    for (const MemberInfo* memberInfo : type->GetMembers())
                    {
                        const TypeInfo* memberType = memberInfo->GetType();

                        // print array structs if needed
                        if (memberType->IsArray())
                        {
                            if (!PrintArrayStruct(outFile, memberType))
                            {
                                return false;
                            }
                        }
                        // forward declare structs if needed
                        else if (memberType->IsPointer() && memberType->GetInnerType()->IsAggregate())
                        {
                            outFile << "struct " << memberType->GetInnerType()->GetShortName() << ";\n\n";
                        }
                    }

                    outFile << "struct " << constDecl->name << "\n{\n";

                    for (const MemberInfo* memberInfo : type->GetMembers())
                    {
                        const TypeInfo* memberType = memberInfo->GetType();

                        outFile << "    ";
                        if (!PrintCType(outFile, memberType, memberInfo->GetName()))
                        {
                            return false;
                        }

                        outFile << ";\n";
                    }

                    outFile << "};\n\n";

                    printedTypeIds.insert({id, constDecl->name});
                }
            }
        }
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

            // check return type
            const TypeInfo* innerReturnType = returnType;
            while (innerReturnType->IsPointer())
            {
                innerReturnType = innerReturnType->GetInnerType();
            }
            if (innerReturnType->IsFunction())
            {
                logger.LogError("C header printer does not support function return types");
                return false;
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

void printVarName(ostream& os, ROString varName)
{
    if (varName.GetSize() > 0)
    {
        os << ' ' << varName;
    }
}

bool CHeaderPrinter::PrintCType(ostream& os, const TypeInfo* type, ROString varName)
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
        ROString shortName = type->GetShortName();

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
    else if (type->IsFloat())
    {
        bool ok = false;
        if (type->GetNumBits() == 32)
        {
            os << "float";
            ok = true;
        }
        else if (type->GetNumBits() == 64)
        {
            os << "double";
            ok = true;
        }

        if (ok)
        {
            printVarName(os, varName);
        }
        else
        {
            logger.LogInternalError("Unknown size '{}' for floating-point type", type->GetNumBits());
        }

        return ok;
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
    else if (type->IsStr())
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
        logger.LogError("C header printer does not support type '{}'", type->GetShortName());

        return false;
    }
}

bool CHeaderPrinter::PrintCFunctionType(std::ostream& os, const TypeInfo* type, ROString varName, unsigned pointerCount)
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
    ROString typeName = arrayType->GetShortName();
    if (arrayTypeNames.find(typeName) == arrayTypeNames.end())
    {
        const TypeInfo* innerType = arrayType->GetInnerType();

        const TypeInfo* testType = innerType;
        while (testType->IsPointer())
        {
            testType = testType->GetInnerType();
        }
        if (testType->IsFunction())
        {
            logger.LogError("C header printer does not support arrays of function pointers");
            return false;
        }

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
        if (!PrintCType(os, innerType, ""))
        {
            return false;
        }
        os << "* Data;\n};\n\n";

        arrayTypeNames.insert(typeName);
    }

    return true;
}
