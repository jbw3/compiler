#include "SyntaxTreePrinter.h"
#include "SyntaxTree.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

const string SyntaxTreePrinter::NODE_TYPE_PROPERTY = "type";

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
    }

    PrintProperty(NODE_TYPE_PROPERTY, "BinaryExpression");
    PrintProperty("operator", op);
    PrintProperty("left", binaryExpression->GetLeftExpression());
    PrintProperty("right", binaryExpression->GetRightExpression());
}

void SyntaxTreePrinter::Visit(Assignment* assignment)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "Assignment");
    PrintProperty("variableName", assignment->GetVariableName());
    PrintProperty("expression", assignment->GetExpression());
}

void SyntaxTreePrinter::Visit(WhileLoop* whileLoop)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "WhileLoop");
    PrintProperty("condition", whileLoop->GetCondition());
    PrintProperty("expression", whileLoop->GetExpression());
}

void SyntaxTreePrinter::Visit(FunctionDefinition* functionDefinition)
{
    BracePrinter printer(*this, "{", "}");

    function<void (VariableDefinition*)> printVarDef = [this](VariableDefinition* def){ PrintVariableDefinition(def); };

    PrintProperty(NODE_TYPE_PROPERTY, "FunctionDefinition");
    PrintProperty("name", functionDefinition->GetName());
    PrintProperty("parameters", functionDefinition->GetParameters(), printVarDef);
    PrintProperty("variableDefinitions", functionDefinition->GetVariableDefinitions(), printVarDef);
    PrintProperty("expression", functionDefinition->GetExpression());
}

void SyntaxTreePrinter::Visit(ModuleDefinition* moduleDefinition)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "ModuleDefinition");
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

void SyntaxTreePrinter::PrintVariableDefinition(const VariableDefinition* variableDefinition)
{
    BracePrinter printer(*this, "{", "}");

    PrintProperty(NODE_TYPE_PROPERTY, "VariableDefinition");
    PrintProperty("name", variableDefinition->GetName());
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
