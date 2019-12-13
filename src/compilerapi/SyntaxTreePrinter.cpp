#include "SyntaxTreePrinter.h"
#include "SyntaxTree.h"
#include "utils.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace SyntaxTree;

const string SyntaxTreePrinter::NODE_TYPE_PROPERTY = "nodeType";

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

    string op;
    switch (unaryExpression->GetOperator())
    {
        case UnaryExpression::eNegative:
            op = "-";
            break;
        case UnaryExpression::eComplement:
            op = "!";
            break;
    }

    PrintProperty(NODE_TYPE_PROPERTY, "UnaryExpression");
    PrintProperty("operator", op);
    PrintProperty("expression", unaryExpression->GetSubExpression());
}

void SyntaxTreePrinter::Visit(BinaryExpression* binaryExpression)
{
    BracePrinter printer(*this, "{", "}");

    string op;
    switch (binaryExpression->GetOperator())
    {
        case BinaryExpression::eEqual:
            op = "==";
            break;
        case BinaryExpression::eNotEqual:
            op = "!=";
            break;
        case BinaryExpression::eLessThan:
            op = "<";
            break;
        case BinaryExpression::eLessThanOrEqual:
            op = "<=";
            break;
        case BinaryExpression::eGreaterThan:
            op = ">";
            break;
        case BinaryExpression::eGreaterThanOrEqual:
            op = ">=";
            break;
        case BinaryExpression::eAdd:
            op = "+";
            break;
        case BinaryExpression::eSubtract:
            op = "-";
            break;
        case BinaryExpression::eMultiply:
            op = "*";
            break;
        case BinaryExpression::eDivide:
            op = "/";
            break;
        case BinaryExpression::eRemainder:
            op = "%";
            break;
        case BinaryExpression::eShiftLeft:
            op = "<<";
            break;
        case BinaryExpression::eShiftRightArithmetic:
            op = ">>";
            break;
        case BinaryExpression::eBitwiseAnd:
            op = "&";
            break;
        case BinaryExpression::eBitwiseXor:
            op = "^";
            break;
        case BinaryExpression::eBitwiseOr:
            op = "|";
            break;
        case BinaryExpression::eLogicalAnd:
            op = "&&";
            break;
        case BinaryExpression::eLogicalOr:
            op = "||";
            break;
        case BinaryExpression::eAssign:
            op = "=";
            break;
        case BinaryExpression::eAddAssign:
            op = "+=";
            break;
        case BinaryExpression::eSubtractAssign:
            op = "==";
            break;
        case BinaryExpression::eMultiplyAssign:
            op = "*=";
            break;
        case BinaryExpression::eDivideAssign:
            op = "/=";
            break;
        case BinaryExpression::eRemainderAssign:
            op = "%=";
            break;
        case BinaryExpression::eShiftLeftAssign:
            op = "<<=";
            break;
        case BinaryExpression::eShiftRightArithmeticAssign:
            op = ">>=";
            break;
        case BinaryExpression::eBitwiseAndAssign:
            op = "&=";
            break;
        case BinaryExpression::eBitwiseXorAssign:
            op = "^=";
            break;
        case BinaryExpression::eBitwiseOrAssign:
            op = "|=";
            break;
    }

    PrintProperty(NODE_TYPE_PROPERTY, "BinaryExpression");
    PrintProperty("operator", op);
    PrintProperty("left", binaryExpression->GetLeftExpression());
    PrintProperty("right", binaryExpression->GetRightExpression());
}

void SyntaxTreePrinter::Visit(WhileLoop* whileLoop)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "WhileLoop");
    PrintProperty("condition", whileLoop->GetCondition());
    PrintProperty("expression", whileLoop->GetExpression());
}

void SyntaxTreePrinter::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const FunctionDeclaration*)> printDecl = [this](const FunctionDeclaration* decl){ PrintFunctionDeclaration(decl); };

    PrintProperty(NODE_TYPE_PROPERTY, "ExternFunctionDeclaration");
    PrintProperty("declaration", externFunctionDeclaration->GetDeclaration(), printDecl);
}

void SyntaxTreePrinter::Visit(FunctionDefinition* functionDefinition)
{
    BracePrinter printer(*this, "{", "}");

    function<void (const FunctionDeclaration*)> printDecl = [this](const FunctionDeclaration* decl){ PrintFunctionDeclaration(decl); };

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionDefinition");
    PrintProperty("declaration", functionDefinition->GetDeclaration(), printDecl);
    PrintProperty("expression", functionDefinition->GetExpression());
}

void SyntaxTreePrinter::Visit(ModuleDefinition* moduleDefinition)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ModuleDefinition");
    PrintProperty("externFunctions", moduleDefinition->GetExternFunctionDeclarations());
    PrintProperty("functions", moduleDefinition->GetFunctionDefinitions());
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
    PrintProperty("value", numericExpression->GetNumber());
}

void SyntaxTreePrinter::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BoolLiteralExpression");
    PrintProperty("value", boolLiteralExpression->GetValue());
}

void SyntaxTreePrinter::Visit(StringLiteralExpression* stringLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    stringstream ss;
    ss << hex;

    const vector<char>& chars = stringLiteralExpression->GetCharacters();
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
            // TODO: this is not valid for all unicode code points

            unsigned int value = 0;
            if (is1ByteUtf8(ch)) // 1-byte UTF-8 sequence
            {
                value |= ch;
            }
            else if (is2ByteUtf8Start(ch)) // 2-byte UTF-8 sequence
            {
                value |= ch & 0x1f;

                if (idx < charsSize - 1)
                {
                    ++idx;
                    ch = chars[idx];

                    // make sure the second byte is valid
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
    PrintProperty("value", ss.str());
}

void SyntaxTreePrinter::Visit(VariableExpression* variableExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "VariableExpression");
    PrintProperty("name", variableExpression->GetName());
}

void SyntaxTreePrinter::Visit(BlockExpression* blockExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BlockExpression");
    PrintProperty("expressions", blockExpression->GetExpressions());
}

void SyntaxTreePrinter::Visit(FunctionExpression* functionExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionExpression");
    PrintProperty("name", functionExpression->GetName());
    PrintProperty("arguments", functionExpression->GetArguments());
}

void SyntaxTreePrinter::Visit(BranchExpression* branchExpression)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "BranchExpression");
    PrintProperty("ifCondition", branchExpression->GetIfCondition());
    PrintProperty("ifExpression", branchExpression->GetIfExpression());
    PrintProperty("elseExpression", branchExpression->GetElseExpression());
}

void SyntaxTreePrinter::Visit(VariableDeclaration* variableDeclaration)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "VariableDeclaration");
    PrintProperty("name", variableDeclaration->GetName());
}

void SyntaxTreePrinter::PrintFunctionDeclaration(const FunctionDeclaration* declaration)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionDeclaration");
    PrintProperty("name", declaration->GetName());
    PrintProperty("parameters", declaration->GetParameters());
}

void SyntaxTreePrinter::PrintProperty(const string& name, const string& value)
{
    if (firstItem)
    {
        firstItem = false;
    }
    else
    {
        Print(",\n");
    }

    Print("\"");
    Print(name);
    Print("\": \"");
    Print(value);
    Print("\"");
}

void SyntaxTreePrinter::PrintProperty(const string& name, SyntaxTreeNode* value)
{
    if (firstItem)
    {
        firstItem = false;
    }
    else
    {
        Print(",\n");
    }

    Print("\"");
    Print(name);
    Print("\":\n");

    value->Accept(this);
}

void SyntaxTreePrinter::Print(const string& str)
{
    for (char ch : str)
    {
        os->put(ch);

        if (ch == '\n')
        {
            string padding(level * 4, ' ');
            *os << padding;
        }
        else if (level == 0 && (ch == '}' || ch == ']'))
        {
            *os << '\n';
        }
    }
}
