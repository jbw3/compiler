#include "SourceGenerator.h"
#include "SyntaxTree.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SourceGenerator::SourceGenerator(const string& outFilename) :
    indentStr("    ")
{
    os = outFilename.empty() ? &cout : new fstream(outFilename, ios_base::out);
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

    if (forLoop->varTypeExpression != nullptr)
    {
        *os << ' ';
        forLoop->varTypeExpression->Accept(this);
    }

    if (forLoop->indexName.GetSize() > 0)
    {
        *os << ", " << forLoop->indexName;
        if (forLoop->indexTypeExpression != nullptr)
        {
            *os << ' ';
            forLoop->indexTypeExpression->Accept(this);
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

void SourceGenerator::Visit(FunctionTypeExpression* functionTypeExpression)
{
    *os << "fun(";

    size_t numParams = functionTypeExpression->paramNames.size();
    for (size_t i = 0; i < numParams; ++i)
    {
        *os << functionTypeExpression->paramNames[i] << ' ';
        functionTypeExpression->paramTypeExpressions[i]->Accept(this);

        if (i < numParams - 1)
        {
            *os << ", ";
        }
    }

    *os << ')';

    Expression* returnTypeExpression = functionTypeExpression->returnTypeExpression;
    if (returnTypeExpression != nullptr)
    {
        *os << ' ';
        returnTypeExpression->Accept(this);
    }
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

void SourceGenerator::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    *os << "struct\n";
    Indent();
    *os << "{\n";
    ++indentLevel;

    for (const MemberDefinition* member : structDefinitionExpression->members)
    {
        Indent();
        *os << member->name << ' ';
        member->typeExpression->Accept(this);
        *os << ",\n";
    }

    --indentLevel;
    Indent();
    *os << "}";
}

void SourceGenerator::Visit(StructInitializationExpression* structInitializationExpression)
{
    structInitializationExpression->structTypeExpression->Accept(this);
    *os << ":\n";
    Indent();
    *os << "{\n";
    ++indentLevel;

    for (const MemberInitialization* memberInit : structInitializationExpression->memberInitializations)
    {
        Indent();
        *os << memberInit->name << " = ";
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

    if (first)
    {
        first = false;
    }
    else
    {
        os->put('\n');
    }
    for (ConstantDeclaration* constDecl : moduleDefinition->constantDeclarations)
    {
        constDecl->Accept(this);
        *os << ";\n";
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

void SourceGenerator::Visit(Modules* modules)
{
    for (ModuleDefinition* module : modules->modules)
    {
        module->Accept(this);
    }
}

void SourceGenerator::Visit(NumericExpression* numericExpression)
{
    *os << numericExpression->token->value;
}

void SourceGenerator::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    *os << floatLiteralExpression->token->value;
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

void SourceGenerator::Visit(IdentifierExpression* identifierExpression)
{
    *os << identifierExpression->name;
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

    for (ConstantDeclaration* constantDeclaration : blockExpression->constantDeclarations)
    {
        Indent();
        constantDeclaration->Accept(this);

        *os << ";\n";
    }

    const SyntaxTreeNodes& statements = blockExpression->statements;
    size_t numStatements = statements.size();
    for (size_t i = 0; i < numStatements - 1; ++i)
    {
        SyntaxTreeNode* statement = statements[i];

        Indent();
        statement->Accept(this);

        if (
            dynamic_cast<BranchExpression*>(statement) == nullptr
         && dynamic_cast<WhileLoop*>(statement) == nullptr
         && dynamic_cast<ForLoop*>(statement) == nullptr
        )
        {
            *os << ";";
        }
        *os << "\n";
    }

    SyntaxTreeNode* lastStatement = statements[numStatements - 1];
    if (dynamic_cast<UnitTypeLiteralExpression*>(lastStatement) == nullptr)
    {
        Indent();
        lastStatement->Accept(this);

        if (dynamic_cast<Return*>(lastStatement) != nullptr)
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
    *os << "cast(";
    castExpression->typeExpression->Accept(this);
    *os << ", ";
    castExpression->subExpression->Accept(this);
    *os << ')';
}

void SourceGenerator::Visit(ImplicitCastExpression* castExpression)
{
    castExpression->subExpression->Accept(this);
}

void SourceGenerator::Visit(FunctionCallExpression* functionCallExpression)
{
    functionCallExpression->functionExpression->Accept(this);

    *os << '(';

    const Expressions& args = functionCallExpression->arguments;
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

void SourceGenerator::Visit(ConstantDeclaration* constantDeclaration)
{
    *os << "const " << constantDeclaration->name;
    if (constantDeclaration->typeExpression != nullptr)
    {
        *os << ' ';
        constantDeclaration->typeExpression->Accept(this);
    }
    *os << " = ";
    constantDeclaration->assignmentExpression->right->Accept(this);
}

void SourceGenerator::Visit(VariableDeclaration* variableDeclaration)
{
    *os << "var " << variableDeclaration->name;
    if (variableDeclaration->typeExpression != nullptr)
    {
        *os << ' ';
        variableDeclaration->typeExpression->Accept(this);
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

        *os << param->name << ' ';
        param->typeExpression->Accept(this);

        if (i != numParams - 1)
        {
            *os << ", ";
        }
    }

    *os << ")";

    const TypeInfo* retType = functionDeclaration->returnType;
    if (!retType->IsUnit())
    {
        *os << ' ';
        functionDeclaration->returnTypeExpression->Accept(this);
    }
}
