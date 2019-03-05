#include "SyntaxTreePrinter.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SyntaxTreePrinter::SyntaxTreePrinter() : level(0)
{
}

void SyntaxTreePrinter::Visit(const Assignment* assignment)
{
    ++level;

    cout << level << ": =\n";

    assignment->GetVariable()->Accept(this);
    assignment->GetExpression()->Accept(this);

    --level;
}

void SyntaxTreePrinter::Visit(const SyntaxTree::BinaryExpression* binaryExpression)
{
    ++level;

    // TODO: print operator
    cout << level << ": TODO: print operator\n";

    binaryExpression->GetLeftExpression()->Accept(this);
    binaryExpression->GetRightExpression()->Accept(this);

    --level;
}

void SyntaxTreePrinter::Visit(const NumericExpression* numericExpression)
{
    cout << (level + 1) << ": " << numericExpression->GetNumber() << "\n";
}

void SyntaxTreePrinter::Visit(const Variable* variable)
{
    cout << (level + 1) << ": " << variable->GetName() << "\n";
}
