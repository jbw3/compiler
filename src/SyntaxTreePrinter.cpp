#include "SyntaxTreePrinter.h"
#include "SyntaxTree.h"
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
    printer.Print(startStr);
}

SyntaxTreePrinter::BracePrinter::~BracePrinter()
{
    --printer.level;
    printer.Print(endStr);
}

SyntaxTreePrinter::SyntaxTreePrinter() : level(0)
{
}

void SyntaxTreePrinter::Visit(const SyntaxTree::BinaryExpression* binaryExpression)
{
    BracePrinter printer(*this, "{", "}");

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

void SyntaxTreePrinter::Visit(const FunctionDefinition* functionDefinition)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"FunctionDefinition\",\n\"name\": \"");
    Print(functionDefinition->GetName());
    Print("\",\n");

    // print parameters
    const vector<VariableDefinition*>& parameters = functionDefinition->GetParameters();
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

    // print code
    Print(",\n\"code\":\n");
    functionDefinition->GetCode()->Accept(this);
}

void SyntaxTreePrinter::Visit(const ModuleDefinition* moduleDefinition)
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

void SyntaxTreePrinter::Visit(const NumericExpression* numericExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"NumericExpression\",\n\"value\": \"");
    Print(numericExpression->GetNumber());
    Print("\"");
}

void SyntaxTreePrinter::Visit(const VariableExpression* variableExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"VariableExpression\",\n\"name\": \"");
    Print(variableExpression->GetName());
    Print("\"");
}

void SyntaxTreePrinter::PrintVariableDefinition(const VariableDefinition* variableDefinition)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"VariableDefinition\",\n\"name\": \"");
    Print(variableDefinition->GetName());
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
        else if ((ch == '}' || ch == ']') && level == 0)
        {
            cout << '\n';
        }
    }
}
