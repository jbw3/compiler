#include "SyntaxTreePrinter.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SyntaxTreePrinter::BracePrinter::BracePrinter(SyntaxTreePrinter& printer) : printer(printer)
{
    ++printer.level;
    printer.Print("{\n");
}

SyntaxTreePrinter::BracePrinter::~BracePrinter()
{
    --printer.level;
    printer.Print("\n}");
}

SyntaxTreePrinter::SyntaxTreePrinter() : level(0)
{
}

void SyntaxTreePrinter::Visit(const Assignment* assignment)
{
    BracePrinter printer(*this);

    Print("\"type\": \"Assignment\",\n\"variable\":\n");

    assignment->GetVariable()->Accept(this);

    Print(",\n\"expression\":\n");

    assignment->GetExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(const SyntaxTree::BinaryExpression* binaryExpression)
{
    BracePrinter printer(*this);

    Print("\"type\": \"BinaryExpression\",\n\"operator\": \"");

    switch (binaryExpression->GetOperator())
    {
        case BinaryExpression::eAdd:
            Print("+");
            break;
        case BinaryExpression::eSubtract:
            Print("-");
            break;
    }

    Print("\",\n\"left\":\n");

    binaryExpression->GetLeftExpression()->Accept(this);

    Print(",\n\"right\":\n");

    binaryExpression->GetRightExpression()->Accept(this);
}

void SyntaxTreePrinter::Visit(const Function* function)
{
    BracePrinter printer(*this);

    Print("\"type\": \"Function\",\n\"name\": \"");
    Print(function->GetName());
    Print("\",\n");

    // TODO: print parameters
    Print("\"parameters\": [],\n");

    // print code
    Print("\"code\":\n");
    function->GetCode()->Accept(this);
}

void SyntaxTreePrinter::Visit(const NumericExpression* numericExpression)
{
    BracePrinter printer(*this);

    Print("\"type\": \"NumericExpression\",\n\"value\": \"");
    Print(numericExpression->GetNumber());
    Print("\"");
}

void SyntaxTreePrinter::Visit(const Variable* variable)
{
    BracePrinter printer(*this);

    Print("\"type\": \"Variable\",\n\"name\": \"");
    Print(variable->GetName());
    Print("\"");
}

void SyntaxTreePrinter::Print(const string& str) const
{
    string padding(level * 4, ' ');

    for (char ch : str)
    {
        cout.put(ch);

        if (ch == '\n')
        {
            cout << padding;
        }
        else if (ch == '}' && level == 0)
        {
            cout << '\n';
        }
    }
}
