#include "SourceGenerator.h"
#include "SyntaxTree.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SourceGenerator::SourceGenerator(const string& outFilename)
{
    os = outFilename.empty() ? &cout : new fstream(outFilename, ios_base::out);
    indentStr = "    ";
    indentLevel = 0;
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
    *os << UnaryExpression::GetOperatorString(unaryExpression->op);
    unaryExpression->subExpression->Accept(this);
}

void SourceGenerator::Visit(BinaryExpression* binaryExpression)
{
    BinaryExpression::EOperator op = binaryExpression->op;
    unsigned opPrecedence = BinaryExpression::GetPrecedence(op);
    Expression* left = binaryExpression->left;

    BinaryExpression* binLeft = dynamic_cast<BinaryExpression*>(left);
    bool leftNeedParens = false;
    if (binLeft != nullptr)
    {
        leftNeedParens = opPrecedence < BinaryExpression::GetPrecedence(binLeft->op);
    }

    if (leftNeedParens)
    {
        *os << '(';
    }

    left->Accept(this);

    if (leftNeedParens)
    {
        *os << ')';
    }

    if (op == BinaryExpression::eSubscript)
    {
        *os << '[';
        binaryExpression->right->Accept(this);
        *os << ']';
    }
    else
    {
        bool printSpaces = (op != BinaryExpression::eClosedRange) & (op != BinaryExpression::eHalfOpenRange);

        if (printSpaces)
        {
            *os << " ";
        }

        *os << BinaryExpression::GetOperatorString(op);

        if (printSpaces)
        {
            *os << " ";
        }

        Expression* right = binaryExpression->right;

        BinaryExpression* binRight = dynamic_cast<BinaryExpression*>(right);
        bool rightNeedParens = false;
        if (binRight != nullptr)
        {
            rightNeedParens = opPrecedence <= BinaryExpression::GetPrecedence(binRight->op);
        }

        if (rightNeedParens)
        {
            *os << '(';
        }

        right->Accept(this);

        if (rightNeedParens)
        {
            *os << ')';
        }
    }
}

void SourceGenerator::Visit(WhileLoop* whileLoop)
{
    *os << "while ";
    whileLoop->condition->Accept(this);
    *os << '\n';
    whileLoop->expression->Accept(this);
}

void SourceGenerator::Visit(ForLoop* forLoop)
{
    *os << "for ";
    *os << forLoop->variableName;

    if (forLoop->variableTypeNameTokens.size() > 0)
    {
        *os << ' ' << forLoop->variableType->GetShortName();
    }

    if (!forLoop->indexName.empty())
    {
        *os << ", " << forLoop->indexName;
        if (forLoop->indexTypeNameTokens.size() > 0)
        {
            *os << ' ' << forLoop->indexType->GetShortName();
        }
    }

    *os << " in ";
    forLoop->iterExpression->Accept(this);
    *os << '\n';
    forLoop->expression->Accept(this);
}

void SourceGenerator::Visit(LoopControl* loopControl)
{
    *os << loopControl->token->value;
}

void SourceGenerator::Visit(Return* ret)
{
    *os << "return ";
    ret->expression->Accept(this);
}

void SourceGenerator::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    *os << "extern ";
    PrintFunctionDeclaration(externFunctionDeclaration->declaration);
    *os << ";\n";
}

void SourceGenerator::Visit(FunctionDefinition* functionDefinition)
{
    PrintFunctionDeclaration(functionDefinition->declaration);
    *os << '\n';
    functionDefinition->expression->Accept(this);
    *os << '\n';
}

void SourceGenerator::Visit(StructDefinition* structDefinition)
{
    *os << "struct " << structDefinition->name << "\n{\n";

    const TypeInfo* structType = structDefinition->type;
    for (const MemberDefinition* member : structDefinition->members)
    {
        const string& memberName = member->name;
        const TypeInfo* memberType = structType->GetMember(memberName)->GetType();
        *os << indentStr << memberName << " " << memberType->GetShortName() << ",\n";
    }

    *os << "}\n";
}

void SourceGenerator::Visit(StructInitializationExpression* structInitializationExpression)
{
    *os << structInitializationExpression->structName << '\n';
    Indent();
    *os << "{\n";
    ++indentLevel;

    for (const MemberInitialization* memberInit : structInitializationExpression->memberInitializations)
    {
        Indent();
        *os << memberInit->name << ": ";
        memberInit->expression->Accept(this);
        *os << ",\n";
    }

    --indentLevel;
    Indent();
    *os << '}';
}

void SourceGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    bool first = true;

    for (StructDefinition* structDef : moduleDefinition->structDefinitions)
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

    for (ExternFunctionDeclaration* externFunDef : moduleDefinition->externFunctionDeclarations)
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

    for (FunctionDefinition* funDef : moduleDefinition->functionDefinitions)
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
    *os << numericExpression->token->value;
}

void SourceGenerator::Visit(UnitTypeLiteralExpression* /*unitTypeLiteralExpression*/)
{
}

void SourceGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    *os << boolLiteralExpression->token->value;
}

void SourceGenerator::Visit(StringLiteralExpression* stringLiteralExpression)
{
    *os << stringLiteralExpression->token->value;
}

void SourceGenerator::Visit(VariableExpression* variableExpression)
{
    *os << variableExpression->name;
}

void SourceGenerator::Visit(ArraySizeValueExpression* arrayExpression)
{
    *os << '[';
    arrayExpression->sizeExpression->Accept(this);
    *os << "; ";
    arrayExpression->valueExpression->Accept(this);
    *os << ']';
}

void SourceGenerator::Visit(ArrayMultiValueExpression* arrayExpression)
{
    *os << '[';

    const Expressions& exprs = arrayExpression->expressions;
    size_t numExprs = exprs.size();
    if (numExprs > 0)
    {
        exprs[0]->Accept(this);

        for (size_t i = 1; i < numExprs; ++i)
        {
            *os << ", ";
            exprs[i]->Accept(this);
        }
    }

    *os << ']';
}

void SourceGenerator::Visit(BlockExpression* blockExpression)
{
    Indent();
    *os << "{\n";
    ++indentLevel;

    const Expressions& exprs = blockExpression->expressions;
    size_t numExprs = exprs.size();
    for (size_t i = 0; i < numExprs - 1; ++i)
    {
        Expression* expr = exprs[i];

        Indent();
        expr->Accept(this);

        if (
            dynamic_cast<BranchExpression*>(expr) == nullptr
         && dynamic_cast<WhileLoop*>(expr) == nullptr
         && dynamic_cast<ForLoop*>(expr) == nullptr
        )
        {
            *os << ";";
        }
        *os << "\n";
    }

    Expression* lastExpr = exprs[numExprs - 1];
    if (dynamic_cast<UnitTypeLiteralExpression*>(lastExpr) == nullptr)
    {
        Indent();
        lastExpr->Accept(this);

        if (dynamic_cast<Return*>(lastExpr) != nullptr)
        {
            *os << ";";
        }
        *os << "\n";
    }

    --indentLevel;
    Indent();
    *os << "}";
}

void SourceGenerator::Visit(CastExpression* castExpression)
{
    *os << "cast(" << castExpression->GetType()->GetShortName() << ", ";
    castExpression->subExpression->Accept(this);
    *os << ')';
}

void SourceGenerator::Visit(ImplicitCastExpression* castExpression)
{
    castExpression->subExpression->Accept(this);
}

void SourceGenerator::Visit(FunctionExpression* functionExpression)
{
    *os << functionExpression->name << "(";

    const Expressions& args = functionExpression->arguments;
    size_t numArgs = args.size();
    if (numArgs > 0)
    {
        args[0]->Accept(this);

        for (size_t i = 1; i < numArgs; ++i)
        {
            *os << ", ";
            args[i]->Accept(this);
        }
    }

    *os << ")";
}

void SourceGenerator::Visit(MemberExpression* memberExpression)
{
    Expression* subExpression = memberExpression->subExpression;

    bool needParens =
        dynamic_cast<UnaryExpression*>(subExpression) != nullptr ||
        dynamic_cast<BinaryExpression*>(subExpression) != nullptr;

    if (needParens)
    {
        *os << '(';
    }

    subExpression->Accept(this);

    if (needParens)
    {
        *os << ')';
    }

    *os << '.' << memberExpression->memberName;
}

void SourceGenerator::Visit(BranchExpression* branchExpression)
{
    *os << "if ";
    branchExpression->ifCondition->Accept(this);
    *os << "\n";
    branchExpression->ifExpression->Accept(this);

    Expression* elseExpr = branchExpression->elseExpression;
    if (dynamic_cast<UnitTypeLiteralExpression*>(elseExpr) != nullptr)
    {
        // do nothing
    }
    else if (dynamic_cast<BranchExpression*>(elseExpr) != nullptr)
    {
        *os << '\n';
        Indent();
        *os << "el";
        elseExpr->Accept(this);
    }
    else
    {
        *os << '\n';
        Indent();
        *os << "else\n";
        elseExpr->Accept(this);
    }
}

void SourceGenerator::Visit(VariableDeclaration* variableDeclaration)
{
    *os << "var " << variableDeclaration->name;
    if (variableDeclaration->typeNameTokens.size() > 0)
    {
        *os << ' ' << variableDeclaration->variableType->GetShortName();
    }
    *os << " = ";
    variableDeclaration->assignmentExpression->right->Accept(this);
}

void SourceGenerator::Indent()
{
    for (unsigned i = 0; i < indentLevel; ++i)
    {
        *os << indentStr;
    }
}

void SourceGenerator::PrintFunctionDeclaration(FunctionDeclaration* functionDeclaration)
{
    *os << "fun " << functionDeclaration->name << "(";

    const Parameters& parameters = functionDeclaration->parameters;
    size_t numParams = parameters.size();
    for (size_t i = 0; i < numParams; ++i)
    {
        const Parameter* param = parameters[i];

        *os << param->name << " " << param->type->GetShortName();

        if (i != numParams - 1)
        {
            *os << ", ";
        }
    }

    *os << ")";

    const TypeInfo* retType = functionDeclaration->returnType;
    if (!retType->IsUnit())
    {
        *os << " " << retType->GetShortName();
    }
}
