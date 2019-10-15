#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzer::SemanticAnalyzer() :
    isError(false)
{
}

bool SemanticAnalyzer::Process(SyntaxTreeNode* syntaxTree)
{
    syntaxTree->Accept(this);
    return !isError;
}

void SemanticAnalyzer::Visit(UnaryExpression* unaryExpression)
{
    Expression* subExpr = unaryExpression->GetSubExpression();
    subExpr->Accept(this);
    if (isError)
    {
        return;
    }

    if (!CheckUnaryOperatorType(unaryExpression->GetOperator(), subExpr->GetType()))
    {
        isError = true;
        return;
    }

    unaryExpression->SetType(subExpr->GetType());
}

bool SemanticAnalyzer::CheckUnaryOperatorType(UnaryExpression::EOperator op, const TypeInfo* subExprType)
{
    bool ok = false;

    switch (op)
    {
        case UnaryExpression::eNegative:
            ok = subExprType->IsInt();
            break;
        case UnaryExpression::eComplement:
            ok = true;
            break;
    }

    if (!ok)
    {
        cerr << "Unary operator does not support type\n";
    }

    return ok;
}

void SemanticAnalyzer::Visit(BinaryExpression* binaryExpression)
{
    Expression* left = binaryExpression->GetLeftExpression();
    Expression* right = binaryExpression->GetRightExpression();

    left->Accept(this);
    if (isError)
    {
        return;
    }

    right->Accept(this);
    if (isError)
    {
        return;
    }

    if (!CheckBinaryOperatorTypes(binaryExpression->GetOperator(), left->GetType(), right->GetType()))
    {
        isError = true;
        return;
    }

    const TypeInfo* resultType = GetBinaryOperatorResultType(binaryExpression->GetOperator(), left->GetType(), right->GetType());
    binaryExpression->SetType(resultType);
}

bool SemanticAnalyzer::CheckBinaryOperatorTypes(BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType)
{
    bool ok = false;

    if ( !(leftType->IsSameAs(*rightType)) && !(leftType->IsInt() && rightType->IsInt()) )
    {
        cerr << "Left and right operands do not have the same type\n";
        ok = false;
    }
    else
    {
        switch (op)
        {
            case BinaryExpression::eEqual:
            case BinaryExpression::eNotEqual:
            case BinaryExpression::eBitwiseAnd:
            case BinaryExpression::eBitwiseXor:
            case BinaryExpression::eBitwiseOr:
                ok = (leftType == rightType) || (leftType->IsInt() && rightType->IsInt());
                break;
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
                ok = leftType->IsBool();
                break;
            case BinaryExpression::eLessThan:
            case BinaryExpression::eLessThanOrEqual:
            case BinaryExpression::eGreaterThan:
            case BinaryExpression::eGreaterThanOrEqual:
            case BinaryExpression::eAdd:
            case BinaryExpression::eSubtract:
            case BinaryExpression::eMultiply:
            case BinaryExpression::eDivide:
            case BinaryExpression::eModulo:
                ok = leftType->IsInt() && rightType->IsInt();
                break;
            case BinaryExpression::eShiftLeft:
            case BinaryExpression::eShiftRightArithmetic:
                // the return type should be the left type, so the right type has to be the
                // same size or smaller
                ok = leftType->IsInt() && rightType->IsInt() && leftType->GetNumBits() >= rightType->GetNumBits();
                break;
        }

        if (!ok)
        {
            cerr << "Binary operator does not support types\n";
        }
    }

    return ok;
}

const TypeInfo* SemanticAnalyzer::GetBinaryOperatorResultType(BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType)
{
    switch (op)
    {
        case BinaryExpression::eEqual:
        case BinaryExpression::eNotEqual:
        case BinaryExpression::eLessThan:
        case BinaryExpression::eLessThanOrEqual:
        case BinaryExpression::eGreaterThan:
        case BinaryExpression::eGreaterThanOrEqual:
        case BinaryExpression::eLogicalAnd:
        case BinaryExpression::eLogicalOr:
            return TypeInfo::BoolType;
        case BinaryExpression::eAdd:
        case BinaryExpression::eSubtract:
        case BinaryExpression::eMultiply:
        case BinaryExpression::eDivide:
        case BinaryExpression::eModulo:
        case BinaryExpression::eBitwiseAnd:
        case BinaryExpression::eBitwiseXor:
        case BinaryExpression::eBitwiseOr:
        {
            if (leftType->IsBool())
            {
                return TypeInfo::BoolType;
            }
            else if (leftType->IsInt() && rightType->IsInt())
            {
                return (leftType->GetNumBits() > rightType->GetNumBits()) ? leftType : rightType;
            }
            else
            {
                cerr << "Internal error: Could not determine result type\n";
                return nullptr;
            }
        }
        case BinaryExpression::eShiftLeft:
        case BinaryExpression::eShiftRightArithmetic:
            return leftType;
    }
}

void SemanticAnalyzer::Visit(SyntaxTree::Assignment* assignment)
{
    // find variable
    const string& varName = assignment->GetVariableName();
    VariableDefinition* varDef = symbolTable.GetVariable(varName);
    if (varDef == nullptr)
    {
        cerr << "Variable \"" << varName << "\" is not defined\n";
        isError = true;
        return;
    }

    // check expression
    Expression* expression = assignment->GetExpression();
    expression->Accept(this);
    if (isError)
    {
        return;
    }

    // check expression against the variable type
    const TypeInfo* exprType = expression->GetType();
    const TypeInfo* varType = varDef->GetType();
    if (exprType != varType)
    {
        if ( !(exprType->IsInt() && varType->IsInt() && exprType->GetNumBits() <= varType->GetNumBits()) )
        {
            cerr << "Expression does not match argument type\n";
            isError = true;
            return;
        }
    }
}

void SemanticAnalyzer::Visit(WhileLoop* whileLoop)
{
    Expression* condition = whileLoop->GetCondition();
    condition->Accept(this);
    if (isError)
    {
        return;
    }

    // ensure condition is a boolean expression
    if (condition->GetType() != TypeInfo::BoolType)
    {
        isError = true;
        cerr << "While loop condition must be a boolean expression\n";
        return;
    }

    // check statements
    CheckStatements(whileLoop->GetStatements());
    if (isError)
    {
        return;
    }
}

void SemanticAnalyzer::Visit(FunctionDefinition* functionDefinition)
{
    // check statements
    CheckStatements(functionDefinition->GetStatements());
    if (isError)
    {
        return;
    }

    // check return expression
    Expression* returnExpression = functionDefinition->GetReturnExpression();
    returnExpression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* returnType = functionDefinition->GetReturnType();
    const TypeInfo* returnExpressionType = returnExpression->GetType();
    if (returnType != returnExpressionType)
    {
        if ( !(returnExpressionType->IsInt() && returnType->IsInt() && returnExpressionType->GetNumBits() <= returnType->GetNumBits()) )
        {
            isError = true;
            cerr << "Function return expression does not equal return type\n";
        }
    }
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    // build a look-up table for all functions
    functions.clear();
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        auto rv = functions.insert({funcDef->GetName(), funcDef});
        if (!rv.second)
        {
            isError = true;
            cerr << "Function \"" << funcDef->GetName() << "\" has already been defined\n";
            return;
        }
    }

    // perform semantic analysis on all functions
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        symbolTable.Push();

        if (!AddVariables(funcDef->GetParameters()) || !AddVariables(funcDef->GetVariableDefinitions()))
        {
            isError = true;
            return;
        }

        funcDef->Accept(this);
        if (isError)
        {
            break;
        }

        symbolTable.Pop();
    }
}

bool SemanticAnalyzer::AddVariables(const VariableDefinitions& varDefs)
{
    for (VariableDefinition* varDef : varDefs)
    {
        bool ok = symbolTable.AddVariable(varDef->GetName(), varDef);
        if (!ok)
        {
            cerr << "Variable \"" << varDef->GetName() << "\" has already been defined\n";
            return false;
        }
    }

    return true;
}

void SemanticAnalyzer::Visit(NumericExpression* numericExpression)
{
    const TypeInfo* minSizeType = numericExpression->GetMinSizeType();
    if (minSizeType == nullptr)
    {
        isError = true;
        cerr << "Internal error: Could not get type for numeric literal\n";
        return;
    }

    numericExpression->SetType(minSizeType);
}

void SemanticAnalyzer::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    boolLiteralExpression->SetType(TypeInfo::BoolType);
}

void SemanticAnalyzer::Visit(VariableExpression* variableExpression)
{
    const string& varName = variableExpression->GetName();
    VariableDefinition* varDef = symbolTable.GetVariable(varName);
    if (varDef == nullptr)
    {
        cerr << "Variable \"" << varName << "\" is not defined\n";
        isError = true;
    }
    else
    {
        variableExpression->SetType(varDef->GetType());
    }
}

void SemanticAnalyzer::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    auto iter = functions.find(funcName);
    if (iter == functions.cend())
    {
        cerr << "Function \"" << funcName << "\" is not defined\n";
        isError = true;
        return;
    }

    const FunctionDefinition* funcDef = iter->second;

    // check argument count
    const vector<Expression*>& args = functionExpression->GetArguments();
    const vector<VariableDefinition*>& params = funcDef->GetParameters();
    if (args.size() != params.size())
    {
        cerr << "Function '" << funcName << "' expected " << params.size() << " arguments but got " << args.size() << "\n";
        isError = true;
        return;
    }

    // process arguments
    for (size_t i = 0; i < args.size(); ++i)
    {
        // set argument type
        Expression* arg = args[i];
        arg->Accept(this);
        if (isError)
        {
            return;
        }

        // check argument against the parameter type
        const VariableDefinition* param = params[i];
        const TypeInfo* argType = arg->GetType();
        const TypeInfo* paramType = param->GetType();
        if (argType != paramType)
        {
            if ( !(argType->IsInt() && paramType->IsInt() && argType->GetNumBits() <= paramType->GetNumBits()) )
            {
                cerr << "Argument does not match parameter type\n";
                isError = true;
                return;
            }
        }
    }

    // set expression's type to the function's return type
    functionExpression->SetType(funcDef->GetReturnType());
}

void SemanticAnalyzer::Visit(BranchExpression* branchExpression)
{
    Expression* ifCondition = branchExpression->GetIfCondition();
    ifCondition->Accept(this);
    if (isError)
    {
        return;
    }

    // ensure if condition is a boolean expression
    if (ifCondition->GetType() != TypeInfo::BoolType)
    {
        isError = true;
        cerr << "If condition must be a boolean expression\n";
        return;
    }

    // check if statements
    CheckStatements(branchExpression->GetIfStatements());
    if (isError)
    {
        return;
    }

    Expression* ifExpression = branchExpression->GetIfExpression();
    ifExpression->Accept(this);
    if (isError)
    {
        return;
    }

    // check else statements
    CheckStatements(branchExpression->GetElseStatements());
    if (isError)
    {
        return;
    }

    Expression* elseExpression = branchExpression->GetElseExpression();
    elseExpression->Accept(this);
    if (isError)
    {
        return;
    }

    // check the "if" and "else" expression return types
    const TypeInfo* ifType = ifExpression->GetType();
    const TypeInfo* elseType = elseExpression->GetType();
    const TypeInfo* resultType = nullptr;
    if (ifType->IsSameAs(*elseType))
    {
        resultType = ifType;
    }
    else if (ifType->IsInt() && elseType->IsInt())
    {
        if (ifType->GetNumBits() >= elseType->GetNumBits())
        {
            resultType = ifType;
        }
        else
        {
            resultType = elseType;
        }
    }
    else
    {
        isError = true;
        cerr << "'if' and 'else' expressions must have the same type\n";
        return;
    }

    // set the branch expression's result type
    branchExpression->SetType(resultType);
}

void SemanticAnalyzer::CheckStatements(const SyntaxTree::Statements& statements)
{
    for (SyntaxTreeNode* statement : statements)
    {
        statement->Accept(this);
        if (isError)
        {
            return;
        }
    }
}
