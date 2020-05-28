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

    const TypeInfo* subExprType = subExpr->GetType();
    bool isInt = subExprType->IsInt();

    bool ok = false;
    const TypeInfo* resultType = nullptr;
    switch (unaryExpression->GetOperator())
    {
        case UnaryExpression::eNegative:
        {
            TypeInfo::ESign sign = subExprType->GetSign();
            if (isInt)
            {
                if (sign == TypeInfo::eSigned)
                {
                    ok = true;
                    resultType = subExpr->GetType();
                }
                else if (sign == TypeInfo::eContextDependent)
                {
                    const NumericLiteralType* subExprLiteralType = dynamic_cast<const NumericLiteralType*>(subExprType);
                    if (subExprLiteralType == nullptr)
                    {
                        logger.LogError("Internal error: Type with context-dependent sign is not a literal type");
                        isError = true;
                        return;
                    }

                    resultType = subExprLiteralType->GetMinSizeType(TypeInfo::eSigned);
                    if (resultType == nullptr)
                    {
                        logger.LogError("Internal error: Could not determine expression result type");
                        isError = true;
                        return;
                    }

                    ok = FixNumericLiteralType(subExpr, resultType);
                    if (!ok)
                    {
                        isError = true;
                        return;
                    }
                }
            }
            break;
        }
        case UnaryExpression::eComplement:
            ok = subExprType->IsSameAs(*TypeInfo::BoolType) || isInt;
            resultType = subExpr->GetType();
            break;
    }

    if (!ok)
    {
        // TODO: Print unary operator in error message
        logger.LogError("Unary operator does not support type '{}'", subExprType->GetShortName());
        isError = true;
        return;
    }

    unaryExpression->SetType(resultType);
}

bool SemanticAnalyzer::CheckUnaryOperatorType(UnaryExpression::EOperator op, const TypeInfo* subExprType)
{
    bool ok = false;

    switch (op)
    {
        case UnaryExpression::eNegative:
        {
            TypeInfo::ESign sign = subExprType->GetSign();
            ok = subExprType->IsInt() && (sign == TypeInfo::eSigned || sign == TypeInfo::eContextDependent);
            break;
        }
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

    if (left->GetType() == nullptr)
    {
        left->Accept(this);
        if (isError)
        {
            return;
        }
    }

    if (right->GetType() == nullptr)
    {
        right->Accept(this);
        if (isError)
        {
            return;
        }
    }

    bool ok = FixNumericLiteralTypes(left, right);
    if (!ok)
    {
        isError = true;
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

    if (!CheckBinaryOperatorTypes(op, left, right))
    {
        isError = true;
        return;
    }

    const TypeInfo* resultType = GetBinaryOperatorResultType(op, left->GetType(), right->GetType());
    binaryExpression->SetType(resultType);
}

unsigned SemanticAnalyzer::GetIntNumBits(const TypeInfo* type)
{
    unsigned numBits = 0;

    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(type);
    if (literalType == nullptr)
    {
        numBits = type->GetNumBits();
    }
    else
    {
        numBits = type->GetNumBits();
    }

    return numBits;
}

bool SemanticAnalyzer::HaveCompatibleSigns(const TypeInfo* leftType, const TypeInfo* rightType)
{
    bool haveCompatibleSigns = false;
    if (leftType->GetSign() == TypeInfo::eContextDependent || rightType->GetSign() == TypeInfo::eContextDependent)
    {
        haveCompatibleSigns = true;
    }
    else
    {
        haveCompatibleSigns = leftType->GetSign() == rightType->GetSign();
    }

    return haveCompatibleSigns;
}

bool SemanticAnalyzer::HaveCompatibleAssignmentSizes(const TypeInfo* leftType, const TypeInfo* rightType)
{
    unsigned rightNumBits = 0;
    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(rightType);

    if (literalType == nullptr)
    {
        rightNumBits = rightType->GetNumBits();
    }
    else
    {
        if (leftType->GetSign() == TypeInfo::eSigned)
        {
            rightNumBits = literalType->GetSignedNumBits();
        }
        else
        {
            rightNumBits = literalType->GetUnsignedNumBits();
        }
    }

    return leftType->GetNumBits() >= rightNumBits;
}

const TypeInfo* SemanticAnalyzer::GetBiggestSizeType(const TypeInfo* type1, const TypeInfo* type2)
{
    const TypeInfo* resultType = nullptr;
    const NumericLiteralType* literalType1 = dynamic_cast<const NumericLiteralType*>(type1);
    const NumericLiteralType* literalType2 = dynamic_cast<const NumericLiteralType*>(type2);

    if (literalType1 != nullptr && literalType2 != nullptr)
    {
        if (literalType1->GetSignedNumBits() >= literalType2->GetSignedNumBits())
        {
            resultType = type1;
        }
        else
        {
            resultType = type2;
        }
    }
    else if (literalType1 != nullptr)
    {
        unsigned type2NumBits = type2->GetNumBits();
        TypeInfo::ESign type2Sign = type2->GetSign();
        if (type2Sign == TypeInfo::eSigned)
        {
            unsigned type1NumBits = literalType1->GetSignedNumBits();
            unsigned numBits = (type1NumBits > type2NumBits) ? type1NumBits : type2NumBits;
            resultType = TypeInfo::GetMinSignedIntTypeForSize(numBits);
        }
        else
        {
            unsigned type1NumBits = literalType1->GetUnsignedNumBits();
            unsigned numBits = (type1NumBits > type2NumBits) ? type1NumBits : type2NumBits;
            resultType = TypeInfo::GetMinUnsignedIntTypeForSize(numBits);
        }
    }
    else if (literalType2 != nullptr)
    {
        unsigned type1NumBits = type1->GetNumBits();
        TypeInfo::ESign type1Sign = type1->GetSign();
        if (type1Sign == TypeInfo::eSigned)
        {
            unsigned type2NumBits = literalType2->GetSignedNumBits();
            unsigned numBits = (type1NumBits > type2NumBits) ? type1NumBits : type2NumBits;
            resultType = TypeInfo::GetMinSignedIntTypeForSize(numBits);
        }
        else
        {
            unsigned type2NumBits = literalType2->GetUnsignedNumBits();
            unsigned numBits = (type1NumBits > type2NumBits) ? type1NumBits : type2NumBits;
            resultType = TypeInfo::GetMinUnsignedIntTypeForSize(numBits);
        }
    }
    else // neither types are NumericLiteralType
    {
        if (type1->GetNumBits() >= type2->GetNumBits())
        {
            resultType = type1;
        }
        else
        {
            resultType = type2;
        }
    }

    return resultType;
}

bool SemanticAnalyzer::FixNumericLiteralType(SyntaxTree::Expression* expr, const TypeInfo* resultType)
{
    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(expr->GetType());
    if (literalType != nullptr && literalType->GetSign() == TypeInfo::eContextDependent)
    {
        const TypeInfo* newType = literalType->GetMinSizeType(resultType->GetSign());
        if (newType == nullptr)
        {
            logger.LogError("Internal error: Could not determine expression result type");
            return false;
        }

        expr->SetType(newType);
    }

    return true;
}

bool SemanticAnalyzer::FixNumericLiteralTypes(SyntaxTree::Expression* expr1, SyntaxTree::Expression* expr2)
{
    const TypeInfo* expr1Type = expr1->GetType();
    const TypeInfo* expr2Type = expr2->GetType();
    const NumericLiteralType* literalType1 = dynamic_cast<const NumericLiteralType*>(expr1Type);
    const NumericLiteralType* literalType2 = dynamic_cast<const NumericLiteralType*>(expr2Type);

    bool ok = true;
    if (literalType1 != nullptr && literalType2 == nullptr)
    {
        ok = FixNumericLiteralType(expr1, expr2Type);
    }
    else if (literalType1 == nullptr && literalType2 != nullptr)
    {
        ok = FixNumericLiteralType(expr2, expr1Type);
    }

    return ok;
}

bool SemanticAnalyzer::CheckBinaryOperatorTypes(BinaryExpression::EOperator op, const Expression* leftExpr, const Expression* rightExpr)
{
    bool ok = false;
    const TypeInfo* leftType = leftExpr->GetType();
    const TypeInfo* rightType = rightExpr->GetType();

    if (leftType->IsBool() && rightType->IsBool())
    {
        switch (op)
        {
            case BinaryExpression::eEqual:
            case BinaryExpression::eNotEqual:
            case BinaryExpression::eBitwiseAnd:
            case BinaryExpression::eBitwiseXor:
            case BinaryExpression::eBitwiseOr:
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
            case BinaryExpression::eAssign:
            case BinaryExpression::eBitwiseAndAssign:
            case BinaryExpression::eBitwiseXorAssign:
            case BinaryExpression::eBitwiseOrAssign:
                ok = true;
                break;
            default:
                ok = false;
                break;
        }
    }
    else if (leftType->IsInt() && rightType->IsInt())
    {
        bool haveCompatibleSigns = HaveCompatibleSigns(leftType, rightType);

        switch (op)
        {
            case BinaryExpression::eEqual:
            case BinaryExpression::eNotEqual:
            case BinaryExpression::eBitwiseAnd:
            case BinaryExpression::eBitwiseXor:
            case BinaryExpression::eBitwiseOr:
            case BinaryExpression::eLessThan:
            case BinaryExpression::eLessThanOrEqual:
            case BinaryExpression::eGreaterThan:
            case BinaryExpression::eGreaterThanOrEqual:
            case BinaryExpression::eAdd:
            case BinaryExpression::eSubtract:
            case BinaryExpression::eMultiply:
            case BinaryExpression::eDivide:
            case BinaryExpression::eRemainder:
                ok = haveCompatibleSigns;
                break;
            case BinaryExpression::eAssign:
            case BinaryExpression::eAddAssign:
            case BinaryExpression::eSubtractAssign:
            case BinaryExpression::eMultiplyAssign:
            case BinaryExpression::eDivideAssign:
            case BinaryExpression::eRemainderAssign:
            case BinaryExpression::eBitwiseAndAssign:
            case BinaryExpression::eBitwiseXorAssign:
            case BinaryExpression::eBitwiseOrAssign:
                ok = haveCompatibleSigns && HaveCompatibleAssignmentSizes(leftType, rightType);
                break;
            case BinaryExpression::eShiftLeft:
            case BinaryExpression::eShiftRightLogical:
            case BinaryExpression::eShiftRightArithmetic:
            case BinaryExpression::eShiftLeftAssign:
            case BinaryExpression::eShiftRightLogicalAssign:
            case BinaryExpression::eShiftRightArithmeticAssign:
                // TODO: Require right op of shift to be unsigned
                ok = true;
                break;
            default:
                ok = false;
                break;
        }
    }
    else if (leftType->IsSameAs(*rightType))
    {
        ok = op == BinaryExpression::eAssign;
    }

    if (!ok)
    {
        // TODO: Need better error message when integer is too big for assignment: var err u8 = 256;
        // TODO: Print binary operator in error message
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
                return GetBiggestSizeType(leftType, rightType);
            }
            else
            {
                cerr << "Internal error: Could not determine result type\n";
                return nullptr;
            }
        }
        case BinaryExpression::eShiftLeft:
        case BinaryExpression::eShiftRightLogical:
        case BinaryExpression::eShiftRightArithmetic:
            return leftType;
        case BinaryExpression::eAssign:
        case BinaryExpression::eAddAssign:
        case BinaryExpression::eSubtractAssign:
        case BinaryExpression::eMultiplyAssign:
        case BinaryExpression::eDivideAssign:
        case BinaryExpression::eRemainderAssign:
        case BinaryExpression::eShiftLeftAssign:
        case BinaryExpression::eShiftRightLogicalAssign:
        case BinaryExpression::eShiftRightArithmeticAssign:
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

    for (const Parameter* param : funcDecl->GetParameters())
    {
        const string& paramName = param->GetName();
        bool ok = symbolTable.AddVariable(paramName, param->GetType());
        if (!ok)
        {
            isError = true;
            logger.LogError("Variable '{}' has already been declared", paramName);
            return;
        }
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
        if ( !(expressionType->IsInt() && returnType->IsInt() && HaveCompatibleSigns(returnType, expressionType) && HaveCompatibleAssignmentSizes(returnType, expressionType)) )
        {
            isError = true;
            logger.LogError("Invalid function return type. Expected '{}' but got '{}'", returnType->GetShortName(), expressionType->GetShortName());
        }
    }
}

void SemanticAnalyzer::Visit(StructDefinition* structDefinition)
{
    const string& structName = structDefinition->GetName();

    AggregateType* newType = new AggregateType(structName);

    for (const MemberDefinition* member : structDefinition->GetMembers())
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
        bool added = newType->AddMember(memberName, memberType, true);
        if (!added)
        {
            delete newType;
            isError = true;
            logger.LogError("Duplicate member '{}' in struct '{}'", memberName, structName);
            return;
        }
    }

    bool added = TypeInfo::RegisterType(newType);
    if (!added)
    {
        delete newType;
        isError = true;
        logger.LogError("Type '{}' has already been defined", structName);
        return;
    }

    structDefinition->SetType(newType);
}

void SemanticAnalyzer::Visit(StructInitializationExpression* structInitializationExpression)
{
    const string& structName = structInitializationExpression->GetStructName();
    const TypeInfo* type = TypeInfo::GetType(structName);
    if (type == nullptr)
    {
        isError = true;
        logger.LogError("'{}' is not a known type", structName);
        return;
    }

    structInitializationExpression->SetType(type);

    unordered_set<string> membersToInit;
    for (auto pair : type->GetMembers())
    {
        membersToInit.insert(pair.first);
    }

    for (MemberInitialization* member : structInitializationExpression->GetMemberInitializations())
    {
        const string& memberName = member->GetName();

        // get member info
        const MemberInfo* memberInfo = type->GetMember(memberName);
        if (memberInfo == nullptr)
        {
            isError = true;
            logger.LogError("Struct '{}' does not have a member named '{}'", structName, memberName);
            return;
        }

        size_t num = membersToInit.erase(memberName);
        if (num == 0)
        {
            isError = true;
            logger.LogError("Member '{}' has already been initialized", memberName);
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
            bool bothAreInts = memberType->IsInt() & exprType->IsInt();
            if ( !(bothAreInts && HaveCompatibleSigns(memberType, exprType) && HaveCompatibleAssignmentSizes(memberType, exprType)) )
            {
                isError = true;
                logger.LogError("Cannot assign expression of type '{}' to member of type '{}'",
                                exprType->GetShortName(), memberType->GetShortName());
                return;
            }
        }
    }

    if (membersToInit.size() > 0)
    {
        auto iter = membersToInit.cbegin();
        string errorMsg = "The following members were not initialized: " + *iter;
        ++iter;
        for (; iter != membersToInit.cend(); ++iter)
        {
            errorMsg += ", ";
            errorMsg += *iter;
        }

        isError = true;
        logger.LogError(errorMsg.c_str());
        return;
    }
}

bool SemanticAnalyzer::SortTypeDefinitions(ModuleDefinition* moduleDefinition)
{
    const vector<StructDefinition*>& structDefs = moduleDefinition->GetStructDefinitions();
    size_t numStructDefs = structDefs.size();

    unordered_map<string, StructDefinition*> nameMap;
    nameMap.reserve(numStructDefs);

    // build map for fast lookup
    for (StructDefinition* structDef : structDefs)
    {
        const string& structName = structDef->GetName();
        auto rv = nameMap.insert({structName, structDef});
        if (!rv.second)
        {
            logger.LogError("Struct '{}' has already been defined", structName);
            return false;
        }
    }

    vector<StructDefinition*> ordered;
    ordered.reserve(numStructDefs);

    unordered_set<string> resolved;
    resolved.reserve(numStructDefs);

    unordered_set<string> dependents;

    // resolve dependencies
    for (StructDefinition* structDef : structDefs)
    {
        const string& structName = structDef->GetName();

        // resolve this struct's dependencies if we have not done so already
        if (resolved.find(structName) == resolved.end())
        {
            bool ok = ResolveDependencies(structDef, nameMap, ordered, resolved, dependents);
            if (!ok)
            {
                return false;
            }
        }
    }

    moduleDefinition->SwapStructDefinitions(ordered);

    return true;
}

bool SemanticAnalyzer::ResolveDependencies(
    StructDefinition* structDef,
    const unordered_map<string, StructDefinition*>& nameMap,
    vector<StructDefinition*>& ordered,
    unordered_set<string>& resolved,
    unordered_set<string>& dependents)
{
    const string& structName = structDef->GetName();

    for (const MemberDefinition* member : structDef->GetMembers())
    {
        const string& memberTypeName = member->GetTypeName();

        // if we have not seen this member's type yet, resolve its dependencies
        if (TypeInfo::GetType(memberTypeName) == nullptr && resolved.find(memberTypeName) == resolved.end())
        {
            // check for a recursive dependency
            auto dependentsIter = dependents.find(memberTypeName);
            if (dependentsIter != dependents.end())
            {
                const string& memberName = member->GetName();
                logger.LogError("In struct '{}', member '{}' with type '{}' creates recursive dependency", structName, memberName, memberTypeName);
                return false;
            }

            dependents.insert(structName);

            auto nameMapIter = nameMap.find(memberTypeName);
            if (nameMapIter == nameMap.end())
            {
                logger.LogError("'{}' is not a known type", memberTypeName);
                return false;
            }

            StructDefinition* memberStruct = nameMapIter->second;
            bool ok = ResolveDependencies(memberStruct, nameMap, ordered, resolved, dependents);
            if (!ok)
            {
                return false;
            }

            dependents.erase(structName);
        }
    }

    ordered.push_back(structDef);
    resolved.insert(structName);

    return true;
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    // sort struct definitions so each comes after any struct definitions it depends on
    bool ok = SortTypeDefinitions(moduleDefinition);
    if (!ok)
    {
        isError = true;
        return;
    }

    // process struct definitions
    for (StructDefinition* structDef : moduleDefinition->GetStructDefinitions())
    {
        structDef->Accept(this);
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

void SemanticAnalyzer::Visit(UnitTypeLiteralExpression* unitTypeLiteralExpression)
{
    unitTypeLiteralExpression->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(NumericExpression* numericExpression)
{
    unsigned minSignedNumBits = numericExpression->GetMinSignedSize();
    unsigned minUnsignedNumBits = numericExpression->GetMinUnsignedSize();
    if (minSignedNumBits == 0 || minUnsignedNumBits == 0)
    {
        isError = true;
        cerr << "Internal error: Could not get type for numeric literal\n";
        return;
    }

    const NumericLiteralType* type = NumericLiteralType::Create(minSignedNumBits, minUnsignedNumBits);
    numericExpression->SetType(type);
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
    const TypeInfo* varType = symbolTable.GetVariableType(varName);
    if (varType == nullptr)
    {
        cerr << "Variable \"" << varName << "\" is not declared in the current scope\n";
        isError = true;
    }
    else
    {
        variableExpression->SetType(varType);
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
        logger.LogError("Function '{}' is not defined", funcName);
        isError = true;
        return;
    }

    const FunctionDeclaration* funcDecl = iter->second;

    // check argument count
    const vector<Expression*>& args = functionExpression->GetArguments();
    const Parameters& params = funcDecl->GetParameters();
    if (args.size() != params.size())
    {
        const char* suffix = (params.size() == 1) ? "" : "s";
        logger.LogError("Function '{}' expected {} argument{} but got {}", funcName, params.size(), suffix, args.size());
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
        const Parameter* param = params[i];
        const TypeInfo* argType = arg->GetType();
        const TypeInfo* paramType = param->GetType();
        if (!argType->IsSameAs(*paramType))
        {
            if ( !(argType->IsInt() && paramType->IsInt() && HaveCompatibleSigns(paramType, argType) && HaveCompatibleAssignmentSizes(paramType, argType)) )
            {
                logger.LogError("Argument type does not match parameter type. Argument: '{}', parameter: '{}'", argType->GetShortName(), paramType->GetShortName());
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
    memberExpression->SetIsAssignable(expr->GetIsAssignable() && member->GetIsAssignable());
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
    else if (ifType->IsInt() && elseType->IsInt() && HaveCompatibleSigns(ifType, elseType))
    {
        resultType = GetBiggestSizeType(ifType, elseType);
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
    BinaryExpression* assignmentExpression = variableDeclaration->GetAssignmentExpression();
    if (assignmentExpression->GetOperator() != BinaryExpression::eAssign)
    {
        isError = true;
        logger.LogError("Internal error: Binary expression in variable declaration is not an assignment");
        return;
    }

    // process right of assignment expression before adding variable to symbol
    // table in order to detect if the variable is referenced before it is assigned
    Expression* rightExpr = assignmentExpression->GetRightExpression();
    rightExpr->Accept(this);
    if (isError)
    {
        return;
    }

    const string& typeName = variableDeclaration->GetTypeName();
    bool inferTypeName = typeName.empty();
    const TypeInfo* type = nullptr;

    // if no type name was given, infer it from the expression
    if (inferTypeName)
    {
        type = rightExpr->GetType();

        // if this is an int literal, set the type to the minimum signed size
        // that will hold the number
        const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(type);
        if (literalType != nullptr)
        {
            type = TypeInfo::GetMinSignedIntTypeForSize(literalType->GetSignedNumBits());
            if (type == nullptr)
            {
                isError = true;
                logger.LogError("Internal error: Could not infer integer literal type");
                return;
            }
        }
    }
    else // get the type from the name given
    {
        type = TypeInfo::GetType(typeName);
        if (type == nullptr)
        {
            isError = true;
            logger.LogError("'{}' is not a known type", typeName);
            return;
        }
    }

    variableDeclaration->SetVariableType(type);

    const string& varName = variableDeclaration->GetName();
    bool ok = symbolTable.AddVariable(varName, variableDeclaration->GetVariableType());
    if (!ok)
    {
        isError = true;
        logger.LogError("Variable '{}' has already been declared", varName);
        return;
    }

    assignmentExpression->Accept(this);
    if (isError)
    {
        return;
    }

    variableDeclaration->SetType(TypeInfo::UnitType);
}

bool SemanticAnalyzer::SetFunctionDeclarationTypes(FunctionDeclaration* functionDeclaration)
{
    unordered_set<string> processedParams;

    // set parameter types
    for (Parameter* param : functionDeclaration->GetParameters())
    {
        const string& paramName = param->GetName();
        if (processedParams.find(paramName) != processedParams.end())
        {
            logger.LogError("Function '{}' has multiple parameters named '{}'", functionDeclaration->GetName(), paramName);
            return false;
        }

        const string& paramTypeName = param->GetTypeName();
        const TypeInfo* paramType = TypeInfo::GetType(paramTypeName);
        if (paramType == nullptr)
        {
            return false;
        }

        param->SetType(paramType);
        processedParams.insert(paramName);
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
