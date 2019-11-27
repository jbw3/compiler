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
            ok = subExprType->IsInt() && subExprType->IsSigned();
            break;
        case UnaryExpression::eComplement:
            ok = subExprType->IsSameAs(*TypeInfo::BoolType) || subExprType->IsInt();
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
                ok = (leftType->IsSameAs(*UnitTypeInfo::BoolType)) || (leftType->IsInt() && rightType->IsInt() && leftType->IsSigned() == rightType->IsSigned());
                break;
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
                ok = leftType->IsBool() && rightType->IsBool();
                break;
            case BinaryExpression::eLessThan:
            case BinaryExpression::eLessThanOrEqual:
            case BinaryExpression::eGreaterThan:
            case BinaryExpression::eGreaterThanOrEqual:
            case BinaryExpression::eAdd:
            case BinaryExpression::eSubtract:
            case BinaryExpression::eMultiply:
            case BinaryExpression::eDivide:
            case BinaryExpression::eRemainder:
                ok = leftType->IsInt() && rightType->IsInt() && leftType->IsSigned() == rightType->IsSigned();
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
        case BinaryExpression::eRemainder:
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

void SemanticAnalyzer::Visit(Assignment* assignment)
{
    // find variable
    const string& varName = assignment->GetVariableName();
    VariableDeclaration* varDecl = symbolTable.GetVariable(varName);
    if (varDecl == nullptr)
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
    const TypeInfo* varType = varDecl->GetType();
    if (!exprType->IsSameAs(*varType))
    {
        if ( !(exprType->IsInt() && varType->IsInt() && exprType->IsSigned() == varType->IsSigned() && exprType->GetNumBits() <= varType->GetNumBits()) )
        {
            cerr << "Expression does not match argument type\n";
            isError = true;
            return;
        }
    }

    // assignment expressions always evaluate to the unit type
    assignment->SetType(TypeInfo::UnitType);
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
    if (!condition->GetType()->IsBool())
    {
        isError = true;
        cerr << "While loop condition must be a boolean expression\n";
        return;
    }

    // check statements
    Expression* expression = whileLoop->GetExpression();
    expression->Accept(this);
    if (isError)
    {
        return;
    }

    if (!expression->GetType()->IsSameAs(*TypeInfo::UnitType))
    {
        isError = true;
        cerr << "While loop block expression must return the unit type\n";
        return;
    }

    // while loop expressions always evaluate to the unit type
    whileLoop->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(ExternFunctionDeclaration* /*externFunctionDeclaration*/)
{
    // nothing to do here
}

void SemanticAnalyzer::Visit(FunctionDefinition* functionDefinition)
{
    const FunctionDeclaration* funcDecl = functionDefinition->GetDeclaration();

    // create new scope for parameters and add them
    Scope scope(symbolTable);

    if (!AddVariables(funcDecl->GetParameters()))
    {
        isError = true;
        return;
    }

    // check return expression
    Expression* expression = functionDefinition->GetExpression();
    expression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* returnType = funcDecl->GetReturnType();
    const TypeInfo* expressionType = expression->GetType();
    if (!returnType->IsSameAs(*expressionType))
    {
        if ( !(expressionType->IsInt() && returnType->IsInt() && expressionType->IsSigned() == returnType->IsSigned() && expressionType->GetNumBits() <= returnType->GetNumBits()) )
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

    for (ExternFunctionDeclaration* externFunc : moduleDefinition->GetExternFunctionDeclarations())
    {
        const FunctionDeclaration* decl = externFunc->GetDeclaration();
        const string& name = decl->GetName();
        auto rv = functions.insert({name, decl});
        if (!rv.second)
        {
            isError = true;
            cerr << "Function \"" << name << "\" has already been defined\n";
            return;
        }
    }

    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        const FunctionDeclaration* decl = funcDef->GetDeclaration();
        const string& name = decl->GetName();
        auto rv = functions.insert({name, decl});
        if (!rv.second)
        {
            isError = true;
            cerr << "Function \"" << name << "\" has already been defined\n";
            return;
        }
    }

    // perform semantic analysis on all functions
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        funcDef->Accept(this);
        if (isError)
        {
            break;
        }
    }
}

bool SemanticAnalyzer::AddVariables(const VariableDeclarations& varDecls)
{
    for (VariableDeclaration* varDecl : varDecls)
    {
        Visit(varDecl);
        if (isError)
        {
            return false;
        }
    }

    return true;
}

void SemanticAnalyzer::Visit(UnitTypeLiteralExpression* unitTypeLiteralExpression)
{
    unitTypeLiteralExpression->SetType(TypeInfo::UnitType);
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
    VariableDeclaration* varDecl = symbolTable.GetVariable(varName);
    if (varDecl == nullptr)
    {
        cerr << "Variable \"" << varName << "\" is not defined\n";
        isError = true;
    }
    else
    {
        variableExpression->SetType(varDecl->GetType());
    }
}

void SemanticAnalyzer::Visit(BlockExpression* blockExpression)
{
    const Expressions& expressions = blockExpression->GetExpressions();
    size_t size = expressions.size();

    if (size == 0)
    {
        isError = true;
        cerr << "Internal error: Block expression has no sub-expressions\n";
    }
    else
    {
        for (Expression* expression : expressions)
        {
            expression->Accept(this);
            if (isError)
            {
                break;
            }
        }

        if (!isError)
        {
            // the block expression's type is the type of its last expression
            blockExpression->SetType(expressions[size - 1]->GetType());
        }
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

    const FunctionDeclaration* funcDecl = iter->second;

    // check argument count
    const vector<Expression*>& args = functionExpression->GetArguments();
    const VariableDeclarations& params = funcDecl->GetParameters();
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
        const VariableDeclaration* param = params[i];
        const TypeInfo* argType = arg->GetType();
        const TypeInfo* paramType = param->GetType();
        if (!argType->IsSameAs(*paramType))
        {
            if ( !(argType->IsInt() && paramType->IsInt() && argType->IsSigned() == paramType->IsSigned() && argType->GetNumBits() <= paramType->GetNumBits()) )
            {
                cerr << "Argument does not match parameter type\n";
                isError = true;
                return;
            }
        }
    }

    // set expression's type to the function's return type
    functionExpression->SetType(funcDecl->GetReturnType());
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
    if (!ifCondition->GetType()->IsBool())
    {
        isError = true;
        cerr << "If condition must be a boolean expression\n";
        return;
    }

    Expression* ifExpression = branchExpression->GetIfExpression();
    ifExpression->Accept(this);
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
    else if (ifType->IsInt() && elseType->IsInt() && ifType->IsSigned() == elseType->IsSigned())
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

void SemanticAnalyzer::Visit(VariableDeclaration* variableDeclaration)
{
    bool ok = symbolTable.AddVariable(variableDeclaration->GetName(), variableDeclaration);
    if (!ok)
    {
        isError = true;
        cerr << "Variable \"" << variableDeclaration->GetName() << "\" has already been defined\n";
        return;
    }

    variableDeclaration->SetType(TypeInfo::UnitType);
}
