#include "SourceGenerator.h"
#include "SyntaxTree.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SourceGenerator::SourceGenerator(const string& outFilename)
{
    os = outFilename.empty() ? &cout : new fstream(outFilename, ios_base::out);
}

SourceGenerator::~SourceGenerator()
{
    if (os != &cout)
    {
        delete os;
    }
}

void SourceGenerator::Flush()
{
    os->flush();
}

void SourceGenerator::Visit(UnaryExpression* unaryExpression)
{
}

void SourceGenerator::Visit(BinaryExpression* binaryExpression)
{
}

void SourceGenerator::Visit(WhileLoop* whileLoop)
{
}

void SourceGenerator::Visit(ForLoop* forLoop)
{
}

void SourceGenerator::Visit(LoopControl* loopControl)
{
}

void SourceGenerator::Visit(Return* ret)
{
}

void SourceGenerator::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    *os << "extern ";
    PrintFunctionDeclaration(externFunctionDeclaration->GetDeclaration());
    *os << ";\n";
}

void SourceGenerator::Visit(FunctionDefinition* functionDefinition)
{
}

void SourceGenerator::Visit(StructDefinition* structDefinition)
{
}

void SourceGenerator::Visit(StructInitializationExpression* structInitializationExpression)
{
}

void SourceGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    bool first = true;

    for (StructDefinition* structDef : moduleDefinition->GetStructDefinitions())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            os->put('\n');
        }

        structDef->Accept(this);
    }

    for (ExternFunctionDeclaration* externFunDef : moduleDefinition->GetExternFunctionDeclarations())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            os->put('\n');
        }

        externFunDef->Accept(this);
    }

    for (FunctionDefinition* funDef : moduleDefinition->GetFunctionDefinitions())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            os->put('\n');
        }

        funDef->Accept(this);
    }
}

void SourceGenerator::Visit(NumericExpression* numericExpression)
{
}

void SourceGenerator::Visit(UnitTypeLiteralExpression* unitTypeLiteralExpression)
{
}

void SourceGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
}

void SourceGenerator::Visit(StringLiteralExpression* stringLiteralExpression)
{
}

void SourceGenerator::Visit(VariableExpression* variableExpression)
{
}

void SourceGenerator::Visit(ArraySizeValueExpression* arrayExpression)
{
}

void SourceGenerator::Visit(ArrayMultiValueExpression* arrayExpression)
{
}

void SourceGenerator::Visit(BlockExpression* blockExpression)
{
}

void SourceGenerator::Visit(CastExpression* castExpression)
{
}

void SourceGenerator::Visit(FunctionExpression* functionExpression)
{
}

void SourceGenerator::Visit(MemberExpression* memberExpression)
{
}

void SourceGenerator::Visit(BranchExpression* branchExpression)
{
}

void SourceGenerator::Visit(VariableDeclaration* variableDeclaration)
{
}

void SourceGenerator::PrintFunctionDeclaration(FunctionDeclaration* functionDeclaration)
{
    *os << "fun " << functionDeclaration->GetName() << "(";

    const Parameters& parameters = functionDeclaration->GetParameters();
    size_t numParams = parameters.size();
    for (size_t i = 0; i < numParams; ++i)
    {
        const Parameter* param = parameters[i];

        *os << param->GetName() << " " << param->GetType()->GetShortName();

        if (i != numParams - 1)
        {
            *os << ", ";
        }
    }

    *os << ")";

    const TypeInfo* retType = functionDeclaration->GetReturnType();
    if (!retType->IsUnit())
    {
        *os << " " << retType->GetShortName();
    }
}
