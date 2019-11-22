#include "SyntaxTreePrinter.h"
#include "SyntaxTree.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

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

    PrintProperty("type", "UnaryExpression");
    PrintProperty("operator", op);
    PrintProperty("expression", unaryExpression->GetSubExpression());
}

void SyntaxTreePrinter::Visit(BinaryExpression* binaryExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"BinaryExpression\",\n\"operator\": \"");

    switch (binaryExpression->GetOperator())
    {
        case BinaryExpression::eEqual:
            Print("==");
            break;
        case BinaryExpression::eNotEqual:
            Print("!=");
            break;
        case BinaryExpression::eLessThan:
            Print("<");
            break;
        case BinaryExpression::eLessThanOrEqual:
            Print("<=");
            break;
        case BinaryExpression::eGreaterThan:
            Print(">");
            break;
        case BinaryExpression::eGreaterThanOrEqual:
            Print(">=");
            break;
        case BinaryExpression::eAdd:
            Print("+");
            break;
        case BinaryExpression::eSubtract:
            Print("-");
            break;
        case BinaryExpression::eMultiply:
            Print("*");
            break;
        case BinaryExpression::eDivide:
            Print("/");
            break;
        case BinaryExpression::eRemainder:
            Print("%");
            break;
        case BinaryExpression::eShiftLeft:
            Print("<<");
            break;
        case BinaryExpression::eShiftRightArithmetic:
            Print(">>");
            break;
        case BinaryExpression::eBitwiseAnd:
            Print("&");
            break;
        case BinaryExpression::eBitwiseXor:
            Print("^");
            break;
        case BinaryExpression::eBitwiseOr:
            Print("|");
            break;
        case BinaryExpression::eLogicalAnd:
            Print("&&");
            break;
        case BinaryExpression::eLogicalOr:
            Print("||");
            break;
    }

    Print("\",\n\"left\":\n");

    binaryExpression->GetLeftExpression()->Accept(this);

    Print(",\n\"right\":\n");

    binaryExpression->GetRightExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(Assignment* assignment)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"Assignment\",\n\"variableName\": \"");
    Print(assignment->GetVariableName());
    Print("\",\n\"expression\":\n");

    // print expression
    assignment->GetExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(WhileLoop* whileLoop)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"WhileLoop\",\n\"condition\":\n");
    whileLoop->GetCondition()->Accept(this);

    Print(",\n\"expression\":\n");

    // print expression
    whileLoop->GetExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(FunctionDefinition* functionDefinition)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"FunctionDefinition\",\n\"name\": \"");
    Print(functionDefinition->GetName());
    Print("\",\n");

    // print parameters
    const VariableDefinitions& parameters = functionDefinition->GetParameters();
    Print("\"parameters\": ");
    if (parameters.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer2(*this, "[", "]");

        for (size_t i = 0; i < parameters.size(); ++i)
        {
            PrintVariableDefinition(parameters[i]);
            if (i < parameters.size() - 1)
            {
                Print(",\n");
            }
        }
    }
    Print(",\n");

    // print variable definitions
    const VariableDefinitions& variableDefinitions = functionDefinition->GetVariableDefinitions();
    Print("\"variableDefinitions\": ");
    if (variableDefinitions.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer3(*this, "[", "]");

        for (size_t i = 0; i < variableDefinitions.size(); ++i)
        {
            PrintVariableDefinition(variableDefinitions[i]);
            if (i < variableDefinitions.size() - 1)
            {
                Print(",\n");
            }
        }
    }

    // print expression
    Print(",\n\"expression\":\n");
    functionDefinition->GetExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(ModuleDefinition* moduleDefinition)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"ModuleDefinition\",\n");

    // print function definitions
    const vector<FunctionDefinition*>& functions = moduleDefinition->GetFunctionDefinitions();
    Print("\"functions\": ");
    if (functions.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer2(*this, "[", "]");

        for (size_t i = 0; i < functions.size(); ++i)
        {
            functions[i]->Accept(this);
            if (i < functions.size() - 1)
            {
                Print(",\n");
            }
        }
    }
}

void SyntaxTreePrinter::Visit(UnitTypeLiteralExpression* /* unitTypeLiteralExpression */)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"UnitTypeLiteralExpression\"");
}

void SyntaxTreePrinter::Visit(NumericExpression* numericExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"NumericExpression\",\n\"value\": \"");
    Print(numericExpression->GetNumber());
    Print("\"");
}

void SyntaxTreePrinter::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"BoolLiteralExpression\",\n\"value\": \"");
    Print(boolLiteralExpression->GetValue());
    Print("\"");
}

void SyntaxTreePrinter::Visit(VariableExpression* variableExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"VariableExpression\",\n\"name\": \"");
    Print(variableExpression->GetName());
    Print("\"");
}

void SyntaxTreePrinter::Visit(BlockExpression* blockExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"BlockExpression\"");
    PrintExpressions("expressions", blockExpression->GetExpressions());
}

void SyntaxTreePrinter::Visit(FunctionExpression* functionExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"FunctionExpression\",\n\"name\": \"");
    Print(functionExpression->GetName());
    Print("\"");

    // print arguments
    PrintExpressions("arguments", functionExpression->GetArguments());
}

void SyntaxTreePrinter::Visit(BranchExpression* branchExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"BranchExpression\",\n\"ifCondition\":\n");
    branchExpression->GetIfCondition()->Accept(this);

    Print(",\n\"ifExpression\":\n");
    branchExpression->GetIfExpression()->Accept(this);

    Print(",\n\"elseExpression\":\n");
    branchExpression->GetElseExpression()->Accept(this);
}

void SyntaxTreePrinter::PrintVariableDefinition(const VariableDefinition* variableDefinition)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"VariableDefinition\",\n\"name\": \"");
    Print(variableDefinition->GetName());
    Print("\"");
}

void SyntaxTreePrinter::PrintExpressions(const string& attributeName, const Expressions& expressions)
{
    Print(",\n\"");
    Print(attributeName);
    Print("\": ");

    if (expressions.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer3(*this, "[", "]");

        for (size_t i = 0; i < expressions.size(); ++i)
        {
            expressions[i]->Accept(this);
            if (i < expressions.size() - 1)
            {
                Print(",\n");
            }
        }
    }
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
        else if ((ch == '}' || ch == ']') && level == 0)
        {
            *os << '\n';
        }
    }
}
