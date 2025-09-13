#include "SyntaxTreePrinter.h"
#include "SyntaxTree.h"
#include "Token.h"
#include "utils.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace SyntaxTree;

const char* SyntaxTreePrinter::NODE_TYPE_PROPERTY = "nodeType";

SyntaxTreePrinter::BracePrinter::BracePrinter(SyntaxTreePrinter& printer, string start,
                                              string end) :
    printer(printer)
{
    startStr = start + "\n";
    endStr = "\n" + end;

    ++printer.level;
    printer.firstItem = true;
    printer.Print(startStr);
}

SyntaxTreePrinter::BracePrinter::~BracePrinter()
{
    --printer.level;
    printer.Print(endStr);

    if (printer.level == 0)
    {
        printer.Print("\n");
    }
}

SyntaxTreePrinter::SyntaxTreePrinter(const string& outFilename) :
    level(0),
    firstItem(true)
{
    os = outFilename.empty() ? &cout : new fstream(outFilename, ios_base::out);
}

SyntaxTreePrinter::~SyntaxTreePrinter()
{
    if (os != &cout)
    {
        delete os;
    }
}

void SyntaxTreePrinter::Visit(UnaryExpression* unaryExpression)
{
    BracePrinter printer(*this, "{", "}");

    const char* op = UnaryExpression::GetOperatorString(unaryExpression->op);

    PrintProperty(NODE_TYPE_PROPERTY, "UnaryExpression");
    PrintProperty("operator", op);
    PrintProperty("expression", unaryExpression->subExpression);
}

void SyntaxTreePrinter::Visit(BinaryExpression* binaryExpression)
{
    BracePrinter printer(*this, "{", "}");

    const char* op = BinaryExpression::GetOperatorString(binaryExpression->op);

    PrintProperty(NODE_TYPE_PROPERTY, "BinaryExpression");
    PrintProperty("operator", op);
    PrintProperty("left", binaryExpression->left);
    PrintProperty("right", binaryExpression->right);
}

void SyntaxTreePrinter::Visit(WhileLoop* whileLoop)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "WhileLoop");
    PrintProperty("condition", whileLoop->condition);
    PrintProperty("expression", whileLoop->expression);
}

void SyntaxTreePrinter::Visit(ForLoop* forLoop)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "ForLoop");
    PrintProperty("variableName", forLoop->variableName);
    PrintProperty("varTypeExpression", forLoop->varTypeExpression);
    PrintProperty("indexName", forLoop->indexName);
    PrintProperty("indexTypeExpression", forLoop->indexTypeExpression);
    PrintProperty("iterExpression", forLoop->iterExpression);
    PrintProperty("expression", forLoop->expression);
}

void SyntaxTreePrinter::Visit(LoopControl* loopControl)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "LoopControl");
    PrintProperty("value", loopControl->token->value);
}

void SyntaxTreePrinter::Visit(Return* ret)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "Return");
    PrintProperty("expression", ret->expression);
}

void SyntaxTreePrinter::Visit(FunctionTypeExpression* functionTypeExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionTypeExpression");
    PrintProperty("paramTypeExpressions", functionTypeExpression->paramTypeExpressions);
    PrintProperty("paramNames", functionTypeExpression->paramNames);
    PrintProperty("returnTypeExpression", functionTypeExpression->returnTypeExpression);
}

void SyntaxTreePrinter::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    BracePrinter printer(*this, "{", "}");

    function<void (FunctionDeclaration*)> printDecl = [this](FunctionDeclaration* decl){ PrintFunctionDeclaration(decl); };

    PrintProperty(NODE_TYPE_PROPERTY, "ExternFunctionDeclaration");
    PrintProperty("declaration", externFunctionDeclaration->declaration, printDecl);
}

void SyntaxTreePrinter::Visit(FunctionDefinition* functionDefinition)
{
    BracePrinter printer(*this, "{", "}");

    function<void (FunctionDeclaration*)> printDecl = [this](FunctionDeclaration* decl){ PrintFunctionDeclaration(decl); };

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionDefinition");
    PrintProperty("declaration", functionDefinition->declaration, printDecl);
    PrintProperty("expression", functionDefinition->expression);
}

void SyntaxTreePrinter::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    BracePrinter printer(*this, "{", "}");

    function<void (MemberDefinition*)> printMemberDef = [this](MemberDefinition* member){ PrintMemberDefinition(member); };

    PrintProperty(NODE_TYPE_PROPERTY, "StructDefinitionExpression");
    PrintProperty("members", structDefinitionExpression->members, printMemberDef);
}

void SyntaxTreePrinter::Visit(StructInitializationExpression* structInitializationExpression)
{
    BracePrinter printer(*this, "{", "}");

    function<void (MemberInitialization*)> printMemberInit = [this](MemberInitialization* member){ PrintMemberInitialization(member); };

    PrintProperty(NODE_TYPE_PROPERTY, "StructInitializationExpression");
    PrintProperty("structTypeExpression", structInitializationExpression->structTypeExpression);
    PrintProperty("memberInitializations", structInitializationExpression->memberInitializations, printMemberInit);
}

void SyntaxTreePrinter::Visit(ModuleDefinition* moduleDefinition)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ModuleDefinition");
    PrintProperty("constantDeclarations", moduleDefinition->constantDeclarations);
    PrintProperty("externFunctions", moduleDefinition->externFunctionDeclarations);
    PrintProperty("functions", moduleDefinition->functionDefinitions);
}

void SyntaxTreePrinter::Visit(Modules* modules)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "Modules");
    PrintProperty("modules", modules->modules);
}

void SyntaxTreePrinter::Visit(UnitTypeLiteralExpression* /* unitTypeLiteralExpression */)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "UnitTypeLiteralExpression");
}

void SyntaxTreePrinter::Visit(NumericExpression* numericExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "NumericExpression");
    PrintProperty("value", numericExpression->token->value);
}

void SyntaxTreePrinter::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "FloatLiteralExpression");
    PrintProperty("value", floatLiteralExpression->token->value);
}

void SyntaxTreePrinter::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BoolLiteralExpression");
    PrintProperty("value", boolLiteralExpression->token->value);
}

void SyntaxTreePrinter::Visit(StringLiteralExpression* stringLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    stringstream ss;
    ss << hex;

    const vector<char>& chars = stringLiteralExpression->characters;
    size_t charsSize = chars.size();
    for (size_t idx = 0; idx < charsSize; ++idx)
    {
        char ch = chars[idx];

        if (ch == '\\')
        {
            ss << "\\\\";
        }
        else if (ch == '"')
        {
            ss << "\\\"";
        }
        else if (isprint(ch))
        {
            ss << ch;
        }
        else if (ch == '\n')
        {
            ss << "\\n";
        }
        else if (ch == '\r')
        {
            ss << "\\r";
        }
        else if (ch == '\t')
        {
            ss << "\\t";
        }
        else
        {
            unsigned continuingByteCount = 0;
            unsigned value = 0;
            if (is1ByteUtf8(ch)) // 1-byte UTF-8 sequence
            {
                value |= ch;
                continuingByteCount = 0;
            }
            else if (is2ByteUtf8Start(ch)) // 2-byte UTF-8 sequence
            {
                value |= ch & 0x1f;
                continuingByteCount = 1;
            }
            else if (is3ByteUtf8Start(ch)) // 3-byte UTF-8 sequence
            {
                value |= ch & 0x0f;
                continuingByteCount = 2;
            }
            else if (is4ByteUtf8Start(ch)) // 4-byte UTF-8 sequence
            {
                value |= ch & 0x07;
                continuingByteCount = 3;
            }

            // add any continuing bytes in a multi-byte UTF-8 sequence
            for (unsigned i = 0; i < continuingByteCount; ++i)
            {
                if (idx < charsSize - 1)
                {
                    ++idx;
                    ch = chars[idx];

                    // make sure the continuing byte is valid
                    if (isUtf8Continuation(ch))
                    {
                        value <<= 6;
                        value |= ch & 0x3f;
                    }
                }
            }

            ss << "\\u"
               << setfill('0') << setw(4)
               << value;
        }
    }

    PrintProperty(NODE_TYPE_PROPERTY, "StringLiteralExpression");
    PrintProperty("value", ss.str().c_str());
}

void SyntaxTreePrinter::Visit(IdentifierExpression* identifierExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "IdentifierExpression");
    PrintProperty("name", identifierExpression->name);
}

void SyntaxTreePrinter::Visit(ArraySizeValueExpression* arrayExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ArraySizeValueExpression");
    PrintProperty("sizeExpressions", arrayExpression->sizeExpression);
    PrintProperty("valueExpression", arrayExpression->valueExpression);
}

void SyntaxTreePrinter::Visit(ArrayMultiValueExpression* arrayExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ArrayMultiValueExpression");
    PrintProperty("expressions", arrayExpression->expressions);
}

void SyntaxTreePrinter::Visit(BlockExpression* blockExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BlockExpression");
    PrintProperty("constantDeclarations", blockExpression->constantDeclarations);
    PrintProperty("statements", blockExpression->statements);
}

void SyntaxTreePrinter::Visit(CastExpression* castExpression)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "CastExpression");
    PrintProperty("typeExpression", castExpression->typeExpression);
    PrintProperty("expression", castExpression->subExpression);
}

void SyntaxTreePrinter::Visit(ImplicitCastExpression* castExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ImplicitCastExpression");
    PrintProperty("expression", castExpression->subExpression);
}

void SyntaxTreePrinter::Visit(FunctionCallExpression* functionCallExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionExpression");
    PrintProperty("functionExpression", functionCallExpression->functionExpression);
    PrintProperty("arguments", functionCallExpression->arguments);
}

void SyntaxTreePrinter::Visit(MemberExpression* memberExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "MemberExpression");
    PrintProperty("expression", memberExpression->subExpression);
    PrintProperty("memberName", memberExpression->memberName);
}

void SyntaxTreePrinter::Visit(BranchExpression* branchExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BranchExpression");
    PrintProperty("ifCondition", branchExpression->ifCondition);
    PrintProperty("ifExpression", branchExpression->ifExpression);
    PrintProperty("elseExpression", branchExpression->elseExpression);
}

void SyntaxTreePrinter::Visit(ConstantDeclaration* constantDeclaration)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "ConstantDeclaration");
    PrintProperty("name", constantDeclaration->name);
    PrintProperty("typeExpression", constantDeclaration->typeExpression);
    PrintProperty("assignmentExpression", constantDeclaration->assignmentExpression);
}

void SyntaxTreePrinter::Visit(VariableDeclaration* variableDeclaration)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "VariableDeclaration");
    PrintProperty("name", variableDeclaration->name);
    PrintProperty("typeExpression", variableDeclaration->typeExpression);
    PrintProperty("assignmentExpression", variableDeclaration->assignmentExpression);
}

void SyntaxTreePrinter::PrintString(ROString str)
{
    Print("\"");
    Print(str);
    Print("\"");
}

void SyntaxTreePrinter::PrintValueSeparator()
{
    Print(",\n");
}

void SyntaxTreePrinter::PrintParameter(const Parameter* parameter)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "Parameter");
    PrintProperty("name", parameter->name);
    PrintProperty("typeExpression", parameter->typeExpression);
}

void SyntaxTreePrinter::PrintFunctionDeclaration(const FunctionDeclaration* declaration)
{
    BracePrinter printer(*this, "{", "}");

    function<void (Parameter*)> printParameter = [this](Parameter* parameter){ PrintParameter(parameter); };

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionDeclaration");
    PrintProperty("name", declaration->name);
    PrintProperty("parameters", declaration->parameters, printParameter);
    PrintProperty("returnTypeExpression", declaration->returnTypeExpression);
}

void SyntaxTreePrinter::PrintMemberDefinition(const MemberDefinition* member)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const Token*)> printTypeName = [this](const Token* token){ PrintString(token->value); };

    PrintProperty(NODE_TYPE_PROPERTY, "MemberDefinition");
    PrintProperty("name", member->name);
    PrintProperty("typeExpression", member->typeExpression);
}

void SyntaxTreePrinter::PrintMemberInitialization(const MemberInitialization* memberInitialization)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "MemberInitialization");
    PrintProperty("name", memberInitialization->name);
    PrintProperty("expression", memberInitialization->expression);
}

void SyntaxTreePrinter::PrintProperty(ROString name, ROString value)
{
    if (firstItem)
    {
        firstItem = false;
    }
    else
    {
        PrintValueSeparator();
    }

    PrintString(name);
    Print(": ");
    PrintString(value);
}

void SyntaxTreePrinter::PrintProperty(ROString name, SyntaxTreeNode* value)
{
    if (firstItem)
    {
        firstItem = false;
    }
    else
    {
        PrintValueSeparator();
    }

    PrintString(name);
    if (value == nullptr)
    {
        Print(": null");
    }
    else
    {
        Print(":\n");
        value->Accept(this);
    }
}

void SyntaxTreePrinter::PrintProperty(ROString name, const vector<ROString>& values)
{
    function<void (ROString)> fun = [this](ROString s){ PrintString(s); };
    PrintProperty(name, values, fun);
}

void SyntaxTreePrinter::Print(const char* str)
{
    for (const char* ptr = str; *ptr != '\0'; ++ptr)
    {
        char ch = *ptr;
        os->put(ch);

        if (ch == '\n')
        {
            for (unsigned i = 0; i < level * 4; ++i)
            {
                os->put(' ');
            }
        }
    }
}

void SyntaxTreePrinter::Print(ROString str)
{
    const char* ptr = str.GetPtr();
    size_t size = str.GetSize();
    for (size_t i = 0; i < size; ++i)
    {
        char ch = ptr[i];
        os->put(ch);

        if (ch == '\n')
        {
            for (unsigned i = 0; i < level * 4; ++i)
            {
                os->put(' ');
            }
        }
    }
}

void SyntaxTreePrinter::Print(const string& str)
{
    for (char ch : str)
    {
        os->put(ch);

        if (ch == '\n')
        {
            for (unsigned i = 0; i < level * 4; ++i)
            {
                os->put(' ');
            }
        }
    }
}
