#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzer::SemanticAnalyzer(ErrorLogger& logger) :
    logger(logger),
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
        logger.LogError("Unary operator does not support type '{}'", subExprType->GetShortName());
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

    BinaryExpression::EOperator op = binaryExpression->GetOperator();

    if (BinaryExpression::IsAssignment(op))
    {
        if (!left->GetIsAssignable())
        {
            cerr << "Cannot assign to expression\n";
            isError = true;
            return;
        }

        left->SetAccessType(Expression::eStore);
    }

    if (!CheckBinaryOperatorTypes(op, left->GetType(), right->GetType()))
    {
        isError = true;
        return;
    }

    const TypeInfo* resultType = GetBinaryOperatorResultType(op, left->GetType(), right->GetType());
    binaryExpression->SetType(resultType);
}

bool SemanticAnalyzer::CheckBinaryOperatorTypes(BinaryExpression::EOperator op, const TypeInfo* leftType, const TypeInfo* rightType)
{
    bool ok = false;
    bool bothAreInts = leftType->IsInt() & rightType->IsInt();

    if ( !(leftType->IsSameAs(*rightType)) && !bothAreInts )
    {
        ok = false;
    }
    else
    {
        bool haveSameSign = leftType->IsSigned() == rightType->IsSigned();

        switch (op)
        {
            case BinaryExpression::eEqual:
            case BinaryExpression::eNotEqual:
            case BinaryExpression::eBitwiseAnd:
            case BinaryExpression::eBitwiseXor:
            case BinaryExpression::eBitwiseOr:
                ok = (leftType->IsBool() && rightType->IsBool()) || (bothAreInts && haveSameSign);
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
                ok = bothAreInts && haveSameSign;
                break;
            case BinaryExpression::eShiftLeft:
            case BinaryExpression::eShiftRightArithmetic:
            case BinaryExpression::eShiftRightLogical:
                // the return type should be the left type, so the right type has to be the
                // same size or smaller
                ok = bothAreInts && leftType->GetNumBits() >= rightType->GetNumBits();
                break;
            case BinaryExpression::eAssign:
                ok = leftType->IsSameAs(*rightType) || (bothAreInts && haveSameSign && leftType->GetNumBits() >= rightType->GetNumBits());
                break;
            case BinaryExpression::eAddAssign:
            case BinaryExpression::eSubtractAssign:
            case BinaryExpression::eMultiplyAssign:
            case BinaryExpression::eDivideAssign:
            case BinaryExpression::eRemainderAssign:
                ok = bothAreInts && haveSameSign && leftType->GetNumBits() >= rightType->GetNumBits();
                break;
            case BinaryExpression::eShiftLeftAssign:
            case BinaryExpression::eShiftRightArithmeticAssign:
            case BinaryExpression::eShiftRightLogicalAssign:
                ok = bothAreInts && leftType->GetNumBits() >= rightType->GetNumBits();
                break;
            case BinaryExpression::eBitwiseAndAssign:
            case BinaryExpression::eBitwiseXorAssign:
            case BinaryExpression::eBitwiseOrAssign:
                ok = (leftType->IsBool() && rightType->IsBool()) || (bothAreInts && haveSameSign && leftType->GetNumBits() >= rightType->GetNumBits());
                break;
        }
    }

    if (!ok)
    {
        logger.LogError("Binary operator does not support types '{}' and '{}'", leftType->GetShortName(), rightType->GetShortName());
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
        case BinaryExpression::eShiftRightLogical:
            return leftType;
        case BinaryExpression::eAssign:
        case BinaryExpression::eAddAssign:
        case BinaryExpression::eSubtractAssign:
        case BinaryExpression::eMultiplyAssign:
        case BinaryExpression::eDivideAssign:
        case BinaryExpression::eRemainderAssign:
        case BinaryExpression::eShiftLeftAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eBitwiseAndAssign:
        case BinaryExpression::eBitwiseXorAssign:
        case BinaryExpression::eBitwiseOrAssign:
            return TypeInfo::UnitType;
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
            logger.LogError("Invalid function return type. Expected '{}' but got '{}'", returnType->GetShortName(), expressionType->GetShortName());
        }
    }
}

void SemanticAnalyzer::Visit(TypeDefinition* typeDefinition)
{
    const string& typeName = typeDefinition->GetName();

    AggregateType* newType = new AggregateType(typeName);

    unsigned index = 0;
    for (const MemberDefinition* member : typeDefinition->GetMembers())
    {
        const string& memberTypeName = member->GetTypeName();
        const TypeInfo* memberType = TypeInfo::GetType(memberTypeName);
        if (memberType == nullptr)
        {
            delete newType;
            isError = true;
            logger.LogError("'{}' is not a known type", memberTypeName);
            return;
        }

        const string& memberName = member->GetName();
        MemberInfo* memberInfo = new MemberInfo(memberName, index, memberType);
        bool added = newType->AddMember(memberInfo);
        if (!added)
        {
            delete newType;
            isError = true;
            logger.LogError("Duplicate member '{}' in type '{}'", memberName, typeName);
            return;
        }

        ++index;
    }

    bool added = TypeInfo::RegisterType(newType);
    if (!added)
    {
        delete newType;
        isError = true;
        logger.LogError("Type '{}' has already been defined", typeName);
        return;
    }

    typeDefinition->SetType(newType);
}

void SemanticAnalyzer::Visit(TypeInitializationExpression* typeInitializationExpression)
{
    const string& typeName = typeInitializationExpression->GetTypeName();
    const TypeInfo* type = TypeInfo::GetType(typeName);
    if (type == nullptr)
    {
        isError = true;
        logger.LogError("'{}' is not a known type", typeName);
        return;
    }

    typeInitializationExpression->SetType(type);

    for (MemberInitialization* member : typeInitializationExpression->GetMemberInitializations())
    {
        // get member info
        const string& memberName = member->GetName();
        const MemberInfo* memberInfo = type->GetMember(memberName);
        if (memberInfo == nullptr)
        {
            isError = true;
            logger.LogError("Type '{}' does not have a member named '{}'", typeName, memberName);
            return;
        }

        // evaluate expression
        Expression* expr = member->GetExpression();
        expr->Accept(this);
        if (isError)
        {
            return;
        }

        // check types
        const TypeInfo* memberType = memberInfo->GetType();
        const TypeInfo* exprType = expr->GetType();
        if (!memberType->IsSameAs(*exprType))
        {
            bool bothAreInts = memberType->IsInt() & memberType->IsInt();
            bool haveSameSign = memberType->IsSigned() == memberType->IsSigned();
            if (!bothAreInts || !haveSameSign || memberType->GetNumBits() < exprType->GetNumBits())
            {
                isError = true;
                logger.LogError("Cannot assign expression of type '{}' to member of type '{}'",
                                exprType->GetShortName(), memberType->GetShortName());
                return;
            }
        }
    }
}

bool SemanticAnalyzer::SortTypeDefinitions(ModuleDefinition* moduleDefinition)
{
    const vector<TypeDefinition*>& typeDefs = moduleDefinition->GetTypeDefinitions();
    size_t numTypeDefs = typeDefs.size();

    unordered_map<string, TypeDefinition*> nameMap;
    nameMap.reserve(numTypeDefs);

    // build map for fast lookup
    for (TypeDefinition* typeDef : typeDefs)
    {
        const string& typeName = typeDef->GetName();
        auto rv = nameMap.insert({typeName, typeDef});
        if (!rv.second)
        {
            logger.LogError("Type '{}' has already been defined", typeName);
            return false;
        }
    }

    vector<TypeDefinition*> ordered;
    ordered.reserve(numTypeDefs);

    unordered_set<string> resolved;
    resolved.reserve(numTypeDefs);

    unordered_set<string> dependents;

    // resolve dependencies
    for (TypeDefinition* typeDef : typeDefs)
    {
        const string& typeName = typeDef->GetName();

        // resolve this type's dependencies if we have not done so already
        if (resolved.find(typeName) == resolved.end())
        {
            bool ok = ResolveDependencies(typeDef, nameMap, ordered, resolved, dependents);
            if (!ok)
            {
                return false;
            }
        }
    }

    moduleDefinition->SwapTypeDefinitions(ordered);

    return true;
}

bool SemanticAnalyzer::ResolveDependencies(
    TypeDefinition* typeDef,
    const unordered_map<string, TypeDefinition*>& nameMap,
    vector<TypeDefinition*>& ordered,
    unordered_set<string>& resolved,
    unordered_set<string>& dependents)
{
    const string& typeName = typeDef->GetName();

    for (const MemberDefinition* member : typeDef->GetMembers())
    {
        const string& memberTypeName = member->GetTypeName();

        // if we have not seen this member's type yet, resolve its dependencies
        if (TypeInfo::GetType(memberTypeName) == nullptr && resolved.find(memberTypeName) == resolved.end())
        {
            // check for a recursive dependency
            auto iter = dependents.find(memberTypeName);
            if (iter != dependents.end())
            {
                const string& memberName = member->GetName();
                logger.LogError("In type '{}', member '{}' with type '{}' creates recursive dependency", typeName, memberName, memberTypeName);
                return false;
            }

            dependents.insert(typeName);

            TypeDefinition* memberType = nameMap.find(memberTypeName)->second;
            bool ok = ResolveDependencies(memberType, nameMap, ordered, resolved, dependents);
            if (!ok)
            {
                return false;
            }

            dependents.erase(typeName);
        }
    }

    ordered.push_back(typeDef);
    resolved.insert(typeName);

    return true;
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    // sort type definitions so each comes after any type definitions it depends on
    bool ok = SortTypeDefinitions(moduleDefinition);
    if (!ok)
    {
        isError = true;
        return;
    }

    // process type definitions
    for (TypeDefinition* typeDef : moduleDefinition->GetTypeDefinitions())
    {
        typeDef->Accept(this);
        if (isError)
        {
            return;
        }
    }

    // build a look-up table for all functions

    functions.clear();

    for (ExternFunctionDeclaration* externFunc : moduleDefinition->GetExternFunctionDeclarations())
    {
        FunctionDeclaration* decl = externFunc->GetDeclaration();

        ok = SetFunctionDeclarationTypes(decl);
        if (!ok)
        {
            isError = true;
            return;
        }

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
        FunctionDeclaration* decl = funcDef->GetDeclaration();

        ok = SetFunctionDeclarationTypes(decl);
        if (!ok)
        {
            isError = true;
            return;
        }

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
            return;
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

void SemanticAnalyzer::Visit(StringLiteralExpression* stringLiteralExpression)
{
    stringLiteralExpression->SetType(TypeInfo::GetStringPointerType());
}

void SemanticAnalyzer::Visit(VariableExpression* variableExpression)
{
    const string& varName = variableExpression->GetName();
    VariableDeclaration* varDecl = symbolTable.GetVariable(varName);
    if (varDecl == nullptr)
    {
        cerr << "Variable \"" << varName << "\" is not declared in the current scope\n";
        isError = true;
    }
    else
    {
        variableExpression->SetType(varDecl->GetVariableType());
        variableExpression->SetIsAssignable(true);
    }
}

void SemanticAnalyzer::Visit(BlockExpression* blockExpression)
{
    // create new scope for block
    Scope scope(symbolTable);

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
        const TypeInfo* paramType = param->GetVariableType();
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

void SemanticAnalyzer::Visit(MemberExpression* memberExpression)
{
    Expression* expr = memberExpression->GetSubExpression();
    expr->Accept(this);
    if (isError)
    {
        return;
    }

    // check if member is available for this type
    const TypeInfo* exprType = expr->GetType();
    const string& memberName = memberExpression->GetMemberName();
    const MemberInfo* member = exprType->GetMember(memberName);
    if (member == nullptr)
    {
        logger.LogError("Type '{}' has no member named '{}'", exprType->GetShortName(), memberName);
        isError = true;
        return;
    }

    memberExpression->SetType(member->GetType());
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
        logger.LogError("'if' and 'else' expressions have mismatching types ('{}' and '{}')",
            ifType->GetShortName(),
            elseType->GetShortName());
        return;
    }

    // set the branch expression's result type
    branchExpression->SetType(resultType);
}

void SemanticAnalyzer::Visit(VariableDeclaration* variableDeclaration)
{
    bool ok = SetVariableDeclarationType(variableDeclaration);
    if (!ok)
    {
        isError = true;
        return;
    }

    ok = symbolTable.AddVariable(variableDeclaration->GetName(), variableDeclaration);
    if (!ok)
    {
        isError = true;
        cerr << "Variable \"" << variableDeclaration->GetName() << "\" has already been declared\n";
        return;
    }

    variableDeclaration->SetType(TypeInfo::UnitType);
}

bool SemanticAnalyzer::SetVariableDeclarationType(VariableDeclaration* variableDeclaration)
{
    const string& typeName = variableDeclaration->GetTypeName();
    const TypeInfo* type = TypeInfo::GetType(typeName);
    if (type == nullptr)
    {
        logger.LogError("'{}' is not a known type", typeName);
        return false;
    }

    variableDeclaration->SetVariableType(type);

    return true;
}

bool SemanticAnalyzer::SetFunctionDeclarationTypes(FunctionDeclaration* functionDeclaration)
{
    // set parameter types
    for (VariableDeclaration* varDecl : functionDeclaration->GetParameters())
    {
        bool ok = SetVariableDeclarationType(varDecl);
        if (!ok)
        {
            return false;
        }
    }

    // set return type
    const TypeInfo* returnType = nullptr;
    const string& returnTypeName = functionDeclaration->GetReturnTypeName();
    if (returnTypeName.empty())
    {
        returnType = TypeInfo::UnitType;
    }
    else
    {
        returnType = TypeInfo::GetType(returnTypeName);
        if (returnType == nullptr)
        {
            logger.LogError("'{}' is not a known type", returnTypeName);
            return false;
        }
    }

    functionDeclaration->SetReturnType(returnType);

    return true;
}
