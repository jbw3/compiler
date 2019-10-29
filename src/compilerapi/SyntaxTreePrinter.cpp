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
    printer.Print(startStr);
}

SyntaxTreePrinter::BracePrinter::~BracePrinter()
{
    --printer.level;
    printer.Print(endStr);
}

SyntaxTreePrinter::SyntaxTreePrinter(const string& outFilename) : level(0)
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

    Print("\"type\": \"UnaryExpression\",\n\"operator\": \"");

    switch (unaryExpression->GetOperator())
    {
        case UnaryExpression::eNegative:
            Print("-");
            break;
        case UnaryExpression::eComplement:
            Print("!");
            break;
    }

    Print("\",\n\"expression\":\n");

    unaryExpression->GetSubExpression()->Accept(this);
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
        case BinaryExpression::eModulo:
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

    PrintStatements("statements", whileLoop->GetStatements());
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

    // print statements
    const Statements& statements = functionDefinition->GetStatements();
    PrintStatements("statements", statements);

    // print return expression
    Print(",\n\"returnExpression\":\n");
    functionDefinition->GetReturnExpression()->Accept(this);
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

void SyntaxTreePrinter::Visit(FunctionExpression* functionExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"FunctionExpression\",\n\"name\": \"");
    Print(functionExpression->GetName());
    Print("\",\n");

    // print arguments
    const vector<Expression*>& arguments = functionExpression->GetArguments();
    Print("\"arguments\": ");
    if (arguments.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer2(*this, "[", "]");

        for (size_t i = 0; i < arguments.size(); ++i)
        {
            arguments[i]->Accept(this);
            if (i < arguments.size() - 1)
            {
                Print(",\n");
            }
        }
    }
}

void SyntaxTreePrinter::Visit(SyntaxTree::BranchExpression* branchExpression)
{
    BracePrinter printer(*this, "{", "}");

    Print("\"type\": \"BranchExpression\",\n\"ifCondition\":\n");
    branchExpression->GetIfCondition()->Accept(this);

    PrintStatements("ifStatements", branchExpression->GetIfStatements());

    Print(",\n\"ifExpression\":\n");
    branchExpression->GetIfExpression()->Accept(this);

    PrintStatements("elseStatements", branchExpression->GetElseStatements());

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

void SyntaxTreePrinter::PrintStatements(const string& name, const vector<SyntaxTreeNode*> statements)
{
    Print(",\n\"");
    Print(name);
    Print("\": ");

    if (statements.size() == 0)
    {
        Print("[]");
    }
    else
    {
        BracePrinter printer3(*this, "[", "]");

        for (size_t i = 0; i < statements.size(); ++i)
        {
            statements[i]->Accept(this);
            if (i < statements.size() - 1)
            {
                Print(",\n");
            }
        }
    }
}

void SyntaxTreePrinter::Print(const string& str) const
{
    string padding(level * 4, ' ');

    for (char ch : str)
    {
        os->put(ch);

        if (ch == '\n')
        {
            *os << padding;
        }
        else if ((ch == '}' || ch == ']') && level == 0)
        {
            *os << '\n';
        }
    }
}
