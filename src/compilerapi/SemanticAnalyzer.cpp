#include "ErrorLogger.h"
#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include <cassert>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzer::SemanticAnalyzer(ErrorLogger& logger) :
    logger(logger),
    isError(false),
    loopLevel(0),
    currentFunction(nullptr)
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

    UnaryExpression::EOperator op = unaryExpression->GetOperator();

    bool ok = false;
    const TypeInfo* resultType = nullptr;
    switch (op)
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
                        logger.LogInternalError("Type with context-dependent sign is not a literal type");
                        isError = true;
                        return;
                    }

                    resultType = subExprLiteralType->GetMinSizeType(TypeInfo::eSigned);
                    if (resultType == nullptr)
                    {
                        logger.LogInternalError("Could not determine expression result type");
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

        case UnaryExpression::eAddressOf:
            ok = subExpr->GetIsStorage();
            subExpr->SetAccessType(Expression::eAddress);
            resultType = TypeInfo::GetPointerToType(subExprType);
            break;

        case UnaryExpression::eDereference:
            ok = subExprType->IsPointer();
            resultType = subExprType->GetInnerType();
            unaryExpression->SetIsStorage(subExpr->GetIsStorage());
            break;
    }

    if (!ok)
    {
        string opString = UnaryExpression::GetOperatorString(op);
        logger.LogError(*unaryExpression->GetOperatorToken(), "Unary operator '{}' does not support type '{}'", opString, subExprType->GetShortName());
        isError = true;
        return;
    }

    unaryExpression->SetType(resultType);
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

    if (
         (left->GetType()->IsInt() && right->GetType()->IsInt())
      || (left->GetType()->IsRange() && right->GetType()->IsRange())
      || (left->GetType()->IsArray() && right->GetType()->IsArray())
    )
    {
        bool ok = FixNumericLiteralTypes(left, right);
        if (!ok)
        {
            isError = true;
            return;
        }
    }

    BinaryExpression::EOperator op = binaryExpression->GetOperator();

    if (BinaryExpression::IsAssignment(op))
    {
        if (!left->GetIsStorage())
        {
            logger.LogError(*binaryExpression->GetOperatorToken(), "Cannot assign to expression");
            isError = true;
            return;
        }

        left->SetAccessType(Expression::eAddress);
    }

    if (!CheckBinaryOperatorTypes(op, left, right, binaryExpression->GetOperatorToken()))
    {
        isError = true;
        return;
    }

    const TypeInfo* resultType = GetBinaryOperatorResultType(op, left->GetType(), right->GetType());
    binaryExpression->SetType(resultType);

    if (op == BinaryExpression::eSubscript && left->GetType()->IsArray() && right->GetType()->IsInt())
    {
        binaryExpression->SetIsStorage(left->GetIsStorage());
    }
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

bool SemanticAnalyzer::HaveCompatibleAssignmentSizes(const TypeInfo* assignType, const TypeInfo* exprType)
{
    unsigned rightNumBits = 0;
    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(exprType);

    if (literalType == nullptr)
    {
        rightNumBits = exprType->GetNumBits();
    }
    else
    {
        if (assignType->GetSign() == TypeInfo::eSigned)
        {
            rightNumBits = literalType->GetSignedNumBits();
        }
        else
        {
            rightNumBits = literalType->GetUnsignedNumBits();
        }
    }

    return assignType->GetNumBits() >= rightNumBits;
}

bool SemanticAnalyzer::AreCompatibleRanges(const TypeInfo* type1, const TypeInfo* type2, const TypeInfo*& outType)
{
    outType = nullptr;

    // make sure both types are ranges and both are closed or both are half-open
    if (type1->IsRange() && type2->IsRange() && type1->IsHalfOpen() == type2->IsHalfOpen())
    {
        const TypeInfo* memberType1 = type1->GetInnerType();
        const TypeInfo* memberType2 = type2->GetInnerType();
        const NumericLiteralType* intLit1 = dynamic_cast<const NumericLiteralType*>(memberType1);
        const NumericLiteralType* intLit2 = dynamic_cast<const NumericLiteralType*>(memberType2);
        if (intLit1 != nullptr && intLit2 != nullptr)
        {
            if (intLit1->GetSignedNumBits() >= intLit2->GetSignedNumBits())
            {
                outType = type1;
            }
            else
            {
                outType = type2;
            }
        }
        else if (intLit1 != nullptr)
        {
            unsigned litNumBits = 0;
            if (memberType2->GetSign() == TypeInfo::eSigned)
            {
                litNumBits = intLit1->GetSignedNumBits();
            }
            else
            {
                litNumBits = intLit1->GetUnsignedNumBits();
            }

            if (memberType2->GetNumBits() >= litNumBits)
            {
                outType = type2;
            }
        }
        else if (intLit2 != nullptr)
        {
            unsigned litNumBits = 0;
            if (memberType1->GetSign() == TypeInfo::eSigned)
            {
                litNumBits = intLit2->GetSignedNumBits();
            }
            else
            {
                litNumBits = intLit2->GetUnsignedNumBits();
            }

            if (memberType1->GetNumBits() >= litNumBits)
            {
                outType = type1;
            }
        }
        else
        {
            if (type1->GetNumBits() == type2->GetNumBits())
            {
                outType = type1;
            }
        }
    }

    return outType != nullptr;
}

const TypeInfo* SemanticAnalyzer::GetBiggestSizeType(const TypeInfo* type1, const TypeInfo* type2)
{
    const TypeInfo* resultType = nullptr;
    const NumericLiteralType* literalType1 = dynamic_cast<const NumericLiteralType*>(type1);
    const NumericLiteralType* literalType2 = dynamic_cast<const NumericLiteralType*>(type2);

    if (literalType1 != nullptr && literalType2 != nullptr)
    {
        unsigned signedNumBits = max(literalType1->GetSignedNumBits(), literalType2->GetSignedNumBits());

        if (literalType1->GetSign() == TypeInfo::eSigned || literalType2->GetSign() == TypeInfo::eSigned)
        {
            resultType = NumericLiteralType::CreateSigned(signedNumBits);
        }
        else
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

const TypeInfo* GetLowestType(const TypeInfo* type, unsigned& arrayLevel, bool& hasRange, bool& hasHalfOpenRange)
{
    arrayLevel = 0;
    while (type->IsArray())
    {
        type = type->GetInnerType();
        ++arrayLevel;
    }

    hasHalfOpenRange = false;
    hasRange = type->IsRange();
    if (hasRange)
    {
        hasHalfOpenRange = type->IsHalfOpen();
        type = type->GetInnerType();
    }

    return type;
}

const TypeInfo* GetLowestType(const TypeInfo* type)
{
    unsigned arrayLevel = 0;
    bool hasRange = false;
    bool hasHalfOpenRange = false;
    return GetLowestType(type, arrayLevel, hasRange, hasHalfOpenRange);
}

bool SemanticAnalyzer::FixNumericLiteralType(Expression* expr, const TypeInfo* resultType)
{
    const TypeInfo* exprType = expr->GetType();
    unsigned arrayLevel = 0;
    bool hasRange = false;
    bool hasHalfOpenRange = false;
    const TypeInfo* lowestType = GetLowestType(exprType, arrayLevel, hasRange, hasHalfOpenRange);
    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(lowestType);

    if ( literalType != nullptr && (literalType->GetSign() == TypeInfo::eContextDependent || arrayLevel > 0) )
    {
        TypeInfo::ESign literalTypeSign = literalType->GetSign();
        const TypeInfo* lowestResultType = GetLowestType(resultType);
        TypeInfo::ESign resultSign = lowestResultType->GetSign();

        const TypeInfo* newType = nullptr;
        if (arrayLevel > 0)
        {
            TypeInfo::ESign sign = TypeInfo::eNotApplicable;
            if (literalTypeSign == TypeInfo::eContextDependent)
            {
                sign = resultSign;
            }
            else
            {
                sign = literalTypeSign;
            }

            bool isSigned = sign == TypeInfo::eSigned;
            unsigned literalNumBits = 0;
            if (isSigned)
            {
                literalNumBits = literalType->GetSignedNumBits();
            }
            else
            {
                literalNumBits = literalType->GetUnsignedNumBits();
            }

            unsigned resultNumBits = lowestResultType->GetNumBits();
            unsigned size = (resultNumBits > literalNumBits) ? resultNumBits : literalNumBits;
            if (isSigned)
            {
                newType = TypeInfo::GetMinSignedIntTypeForSize(size);
            }
            else
            {
                newType = TypeInfo::GetMinUnsignedIntTypeForSize(size);
            }
        }
        else
        {
            newType = literalType->GetMinSizeType(resultSign);
            if (newType == nullptr)
            {
                logger.LogInternalError("Could not determine expression result type");
                return false;
            }
        }

        if (hasRange)
        {
            newType = TypeInfo::GetRangeType(newType, hasHalfOpenRange);
        }
        // TODO: This does not work with nested arrays of integer literals.
        // Need to set the expression type at each level
        for (unsigned i = 0; i < arrayLevel; ++i)
        {
            newType = TypeInfo::GetArrayOfType(newType);
        }

        expr->SetType(newType);
    }

    return true;
}

bool SemanticAnalyzer::FixNumericLiteralTypes(Expression* expr1, Expression* expr2)
{
    const TypeInfo* expr1Type = expr1->GetType();
    const TypeInfo* expr2Type = expr2->GetType();
    const NumericLiteralType* literalType1 = dynamic_cast<const NumericLiteralType*>(GetLowestType(expr1Type));
    const NumericLiteralType* literalType2 = dynamic_cast<const NumericLiteralType*>(GetLowestType(expr2Type));

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

bool SemanticAnalyzer::CheckBinaryOperatorTypes(BinaryExpression::EOperator op, Expression* leftExpr, Expression* rightExpr, const Token* opToken)
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
            case BinaryExpression::eClosedRange:
            case BinaryExpression::eHalfOpenRange:
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
    else if (leftType->IsPointer() && rightType->IsPointer())
    {
        ok =
            (op == BinaryExpression::eAssign || op == BinaryExpression::eEqual || op == BinaryExpression::eNotEqual)
            && leftType->IsSameAs(*rightType);
    }
    else if (leftType->IsSameAs(*TypeInfo::GetStringType()))
    {
        if (op == BinaryExpression::eAssign)
        {
            ok = leftType->IsSameAs(*rightType);
        }
        else if (op == BinaryExpression::eSubscript && rightType->IsInt())
        {
            TypeInfo::ESign rightSign = rightType->GetSign();
            if (rightSign == TypeInfo::eUnsigned)
            {
                ok = true;
            }
            else if (rightSign == TypeInfo::eContextDependent)
            {
                const TypeInfo* newType = TypeInfo::GetMinUnsignedIntTypeForSize(rightType->GetNumBits());
                rightExpr->SetType(newType);

                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        else if (op == BinaryExpression::eSubscript && rightType->IsRange())
        {
            const TypeInfo* innerType = rightType->GetInnerType();
            TypeInfo::ESign rightSign = innerType->GetSign();
            if (rightSign == TypeInfo::eUnsigned)
            {
                ok = true;
            }
            else if (rightSign == TypeInfo::eContextDependent)
            {
                const TypeInfo* newInnerType = TypeInfo::GetMinUnsignedIntTypeForSize(innerType->GetNumBits());
                const TypeInfo* newType = TypeInfo::GetRangeType(newInnerType, rightType->IsHalfOpen());
                rightExpr->SetType(newType);

                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    else if (leftType->IsArray())
    {
        if (op == BinaryExpression::eAssign && rightType->IsArray())
        {
            if (leftType->IsSameAs(*rightType))
            {
                ok = true;
            }
            else
            {
                const TypeInfo* leftInnerType = leftType->GetInnerType();
                const TypeInfo* rightInnerType = rightType->GetInnerType();
                if (leftInnerType->IsInt() && rightInnerType->IsInt()
                 && HaveCompatibleSigns(leftInnerType, rightInnerType)
                 && leftInnerType->GetNumBits() == rightInnerType->GetNumBits())
                {
                    ok = true;
                }
                else
                {
                    ok = false;
                }
            }
        }
        else if (op == BinaryExpression::eSubscript && rightType->IsInt())
        {
            TypeInfo::ESign rightSign = rightType->GetSign();
            if (rightSign == TypeInfo::eUnsigned)
            {
                ok = true;
            }
            else if (rightSign == TypeInfo::eContextDependent)
            {
                const TypeInfo* newType = TypeInfo::GetMinUnsignedIntTypeForSize(rightType->GetNumBits());
                rightExpr->SetType(newType);

                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        else if (op == BinaryExpression::eSubscript && rightType->IsRange())
        {
            const TypeInfo* innerType = rightType->GetInnerType();
            TypeInfo::ESign rightSign = innerType->GetSign();
            if (rightSign == TypeInfo::eUnsigned)
            {
                ok = true;
            }
            else if (rightSign == TypeInfo::eContextDependent)
            {
                const TypeInfo* newInnerType = TypeInfo::GetMinUnsignedIntTypeForSize(innerType->GetNumBits());
                const TypeInfo* newType = TypeInfo::GetRangeType(newInnerType, rightType->IsHalfOpen());
                rightExpr->SetType(newType);

                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    else if (leftType->IsRange() && rightType->IsRange())
    {
        // assignment is the only valid operator for ranges
        if (op != BinaryExpression::eAssign)
        {
            ok = false;
        }
        // make sure the ranges' half-open/closed flag matches
        else if (leftType->IsHalfOpen() != rightType->IsHalfOpen())
        {
            ok = false;
        }
        else
        {
            const TypeInfo* leftIntType = leftType->GetInnerType();
            const TypeInfo* rightIntType = rightType->GetInnerType();

            // if sizes and signs are the same, we're good
            if (leftType->GetNumBits() == rightType->GetNumBits() && leftIntType->GetSign() == rightIntType->GetSign())
            {
                ok = true;
            }
            else
            {
                // if the right type is a literal, and it's the same size or smaller, then we're good
                const NumericLiteralType* rightIntLitType = dynamic_cast<const NumericLiteralType*>(rightIntType);
                if (rightIntLitType != nullptr && leftIntType->GetNumBits() >= rightIntLitType->GetNumBits())
                {
                    ok = true;
                }
                else
                {
                    ok = false;
                }
            }
        }
    }
    else if (leftType->IsSameAs(*rightType))
    {
        ok = op == BinaryExpression::eAssign;
    }

    if (!ok)
    {
        // TODO: Need better error message when integer is too big for assignment: var err u8 = 256;
        string opString = BinaryExpression::GetOperatorString(op);
        logger.LogError(*opToken, "Binary operator '{}' does not support types '{}' and '{}'", opString, leftType->GetShortName(), rightType->GetShortName());
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
                logger.LogInternalError("Could not determine result type");
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
        case BinaryExpression::eClosedRange:
        case BinaryExpression::eHalfOpenRange:
        {
            const TypeInfo* memberType = GetBiggestSizeType(leftType, rightType);
            return TypeInfo::GetRangeType(memberType, op == BinaryExpression::eHalfOpenRange);
        }
        case BinaryExpression::eSubscript:
        {
            if (rightType->IsInt())
            {
                if (leftType->IsSameAs(*TypeInfo::GetStringType()))
                {
                    return TypeInfo::UInt8Type;
                }
                else if (leftType->IsArray())
                {
                    return leftType->GetInnerType();
                }
                else
                {
                    logger.LogInternalError("Could not determine result type");
                    return nullptr;
                }
            }
            else if (rightType->IsRange())
            {
                // this is a slice, so the result type will be the same as what we are slicing
                return leftType;
            }
            else
            {
                logger.LogInternalError("Could not determine result type");
                return nullptr;
            }
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
    if (!condition->GetType()->IsBool())
    {
        isError = true;
        logger.LogError(*whileLoop->GetWhileToken(), "While loop condition must be a boolean expression");
        return;
    }

    // check statements
    BlockExpression* expression = whileLoop->GetExpression();
    ++loopLevel;
    expression->Accept(this);
    --loopLevel;
    if (isError)
    {
        return;
    }

    if (!expression->GetType()->IsSameAs(*TypeInfo::UnitType))
    {
        isError = true;
        const Token* endBlockToken = expression->GetEndToken();
        logger.LogError(*endBlockToken, "While loop block expression must return the unit type");
        return;
    }

    // while loop expressions always evaluate to the unit type
    whileLoop->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(ForLoop* forLoop)
{
    // process iterable expression
    Expression* iterExpression = forLoop->GetIterExpression();
    iterExpression->Accept(this);
    if (isError)
    {
        return;
    }

    // ensure iterable expression is a range or an array and get the iterator type
    const TypeInfo* iterExprType = iterExpression->GetType();
    const TypeInfo* inferType = nullptr;
    if (iterExprType->IsRange())
    {
        inferType = iterExprType->GetInnerType();
    }
    else if (iterExprType->IsArray())
    {
        inferType = iterExprType->GetInnerType();
    }
    else
    {
        isError = true;
        logger.LogError(*forLoop->GetForToken(), "For loop expression is not iterable");
        return;
    }

    // set variable type
    const TypeInfo* varType = GetVariableType(forLoop->GetVariableTypeNameTokens(), inferType);
    if (isError)
    {
        return;
    }
    forLoop->SetVariableType(varType);

    // set index type if there is an index variable
    const TypeInfo* indexVarType = GetVariableType(forLoop->indexTypeNameTokens, TypeInfo::GetUIntSizeType());
    if (isError)
    {
        return;
    }
    else if (!indexVarType->IsInt() || indexVarType->GetSign() != TypeInfo::eUnsigned || indexVarType->GetNumBits() < TypeInfo::GetUIntSizeType()->GetNumBits())
    {
        isError = true;
        logger.LogError(*forLoop->indexNameToken, "Index variable must be an unsigned integer at least as big as 'usize'");
        return;
    }
    forLoop->indexType = indexVarType;

    // update array type if needed
    if (iterExprType->IsArray())
    {
        const TypeInfo* arrayInnerType = iterExprType->GetInnerType();
        if (arrayInnerType->GetSign() == TypeInfo::eContextDependent)
        {
            const NumericLiteralType* arrayInnerLiteralType = dynamic_cast<const NumericLiteralType*>(arrayInnerType);
            if (arrayInnerLiteralType == nullptr)
            {
                logger.LogInternalError("Type with context-dependent sign is not a literal type");
                isError = true;
                return;
            }

            inferType = arrayInnerLiteralType->GetMinSizeType(varType->GetSign());
            if (inferType == nullptr)
            {
                logger.LogInternalError("Could not determine expression result type");
                isError = true;
                return;
            }

            const TypeInfo* newType = TypeInfo::GetArrayOfType(inferType);
            iterExpression->SetType(newType);
        }
    }

    // create new scope for variable
    Scope scope(symbolTable);

    // add the variable name to the symbol table
    const string& varName = forLoop->GetVariableName();
    bool ok = symbolTable.AddVariable(varName, forLoop->GetVariableType());
    if (!ok)
    {
        isError = true;
        logger.LogError(*forLoop->GetVariableNameToken(), "Variable '{}' has already been declared", varName);
        return;
    }

    // add the index variable name to the symbol table
    const string& indexVarName = forLoop->indexName;
    if (!indexVarName.empty())
    {
        ok = symbolTable.AddVariable(indexVarName, forLoop->indexType);
        if (!ok)
        {
            isError = true;
            logger.LogError(*forLoop->variableNameToken, "Variable '{}' has already been declared", indexVarName);
            return;
        }
    }

    // check variable type and iterable type
    if (!varType->IsSameAs(*inferType) && (!HaveCompatibleSigns(varType, inferType) || !HaveCompatibleAssignmentSizes(varType, inferType)))
    {
        isError = true;
        logger.LogError(
            *forLoop->GetVariableNameToken(),
            "Cannot assign value of type '{}' to variable '{}' of type '{}'",
            inferType->GetShortName(),
            varName,
            varType->GetShortName()
        );
        return;
    }

    // process body expression
    BlockExpression* expression = forLoop->GetExpression();
    ++loopLevel;
    expression->Accept(this);
    --loopLevel;
    if (isError)
    {
        return;
    }

    if (!expression->GetType()->IsSameAs(*TypeInfo::UnitType))
    {
        isError = true;
        const Token* endBlockToken = expression->GetEndToken();
        logger.LogError(*endBlockToken, "For loop block expression must return the unit type");
        return;
    }

    // for loop expressions always evaluate to the unit type
    forLoop->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(LoopControl* loopControl)
{
    // make sure we're in a loop
    if (loopLevel == 0)
    {
        isError = true;
        const Token* token = loopControl->GetToken();
        logger.LogError(*token, "'{}' can only be used in a loop", token->value);
        return;
    }

    // loop control expressions always evaluate to the unit type
    loopControl->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(Return* ret)
{
    // process expression
    Expression* expression = ret->expression;
    expression->Accept(this);
    if (isError)
    {
        return;
    }

    if (currentFunction == nullptr)
    {
        isError = true;
        logger.LogError(*ret->token, "'return' can only be used in a function");
        return;
    }

    // check if expression type matchine function return type
    bool ok = CheckReturnType(currentFunction->GetDeclaration(), expression, ret->token);
    if (!ok)
    {
        isError = true;
        return;
    }

    // return expressions always evaluate to the unit type
    ret->SetType(TypeInfo::UnitType);
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
            const Token* paramToken = param->GetNameToken();
            logger.LogError(*paramToken, "Variable '{}' has already been declared", paramName);
            return;
        }
    }

    Expression* expression = functionDefinition->GetExpression();
    currentFunction = functionDefinition;
    expression->Accept(this);
    currentFunction = nullptr;
    if (isError)
    {
        return;
    }

    // check if the function ends with a return statement
    bool endsWithReturn = false;
    BlockExpression* blockExpr = dynamic_cast<BlockExpression*>(expression);
    assert(blockExpr != nullptr && "Expected function body to be a BlockExpression");
    Expression* lastExpr = blockExpr->GetExpressions().back();
    Return* ret = dynamic_cast<Return*>(lastExpr);
    if (ret != nullptr)
    {
        endsWithReturn = true;
    }
    functionDefinition->endsWithReturnStatement = endsWithReturn;

    // if the function does not end with a return statement, then the last
    // expression will be the return value, and we need to check its type
    if (!endsWithReturn)
    {
        // check last expression
        bool ok = CheckReturnType(funcDecl, expression, blockExpr->GetEndToken());
        if (!ok)
        {
            isError = true;
            return;
        }
    }
}

void SemanticAnalyzer::Visit(StructDefinition* structDefinition)
{
    const string& structName = structDefinition->GetName();

    auto iter = partialStructTypes.find(structName);
    assert(iter != partialStructTypes.cend());
    AggregateType* newType = iter->second;

    for (const MemberDefinition* member : structDefinition->GetMembers())
    {
        const TypeInfo* memberType = NameToType(member->GetTypeNameTokens());
        if (memberType == nullptr)
        {
            delete newType;
            isError = true;
            return;
        }

        const string& memberName = member->GetName();
        bool added = newType->AddMember(memberName, memberType, true, member->GetNameToken());
        if (!added)
        {
            delete newType;
            isError = true;
            logger.LogError(*member->GetNameToken(), "Duplicate member '{}' in struct '{}'", memberName, structName);
            return;
        }
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
        const Token* token = structInitializationExpression->GetStructNameToken();
        logger.LogError(*token, "'{}' is not a known type", structName);
        return;
    }

    structInitializationExpression->SetType(type);

    unordered_set<string> membersToInit;
    for (const MemberInfo* member : type->GetMembers())
    {
        membersToInit.insert(member->GetName());
    }

    for (MemberInitialization* member : structInitializationExpression->GetMemberInitializations())
    {
        const string& memberName = member->GetName();

        // get member info
        const MemberInfo* memberInfo = type->GetMember(memberName);
        if (memberInfo == nullptr)
        {
            isError = true;
            const Token* token = member->GetNameToken();
            logger.LogError(*token, "Struct '{}' does not have a member named '{}'", structName, memberName);
            return;
        }

        size_t num = membersToInit.erase(memberName);
        if (num == 0)
        {
            isError = true;
            const Token* token = member->GetNameToken();
            logger.LogError(*token, "Member '{}' has already been initialized", memberName);
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
                const Token* token = member->GetNameToken();
                logger.LogError(*token, "Cannot assign expression of type '{}' to member '{}' of type '{}'",
                                exprType->GetShortName(), memberName, memberType->GetShortName());
                return;
            }
        }
    }

    // error if not all members were initialized
    size_t membersNotInit = membersToInit.size();
    if (membersNotInit > 0)
    {
        string errorMsg;
        if (membersNotInit == 1)
        {
            errorMsg = "Struct member '" + *membersToInit.cbegin() + "' was not initialized";
        }
        else
        {
            auto iter = membersToInit.cbegin();
            errorMsg = "The following struct members were not initialized: " + *iter;
            ++iter;
            for (; iter != membersToInit.cend(); ++iter)
            {
                errorMsg += ", ";
                errorMsg += *iter;
            }
        }

        isError = true;
        const Token* token = structInitializationExpression->GetStructNameToken();
        logger.LogError(*token, errorMsg.c_str());
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
            logger.LogError(*structDef->GetNameToken(), "Struct '{}' has already been defined", structName);
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
        const vector<const Token*>& typeNameTokens = member->GetTypeNameTokens();

        size_t size = typeNameTokens.size();

        // we only need to check types that might be structs
        // (which will only have 1 token as a type name)
        if (size == 1)
        {
            const Token* token = typeNameTokens[0];
            const string& memberTypeName = token->value;

            // if we have not seen this member's type yet, resolve its dependencies
            if (TypeInfo::GetType(memberTypeName) == nullptr && resolved.find(memberTypeName) == resolved.end())
            {
                // check for a recursive dependency
                auto dependentsIter = dependents.find(memberTypeName);
                if (dependentsIter != dependents.end())
                {
                    const string& memberName = member->GetName();
                    logger.LogError(*member->GetNameToken(), "In struct '{}', member '{}' with type '{}' creates recursive dependency", structName, memberName, memberTypeName);
                    return false;
                }

                dependents.insert(structName);

                auto nameMapIter = nameMap.find(memberTypeName);
                if (nameMapIter == nameMap.end())
                {
                    logger.LogError(*token, "'{}' is not a known type", memberTypeName);
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
    }

    ordered.push_back(structDef);
    resolved.insert(structName);

    // register the type name. we'll add its members later
    AggregateType* newType = new AggregateType(structName, structDef->GetNameToken());
    partialStructTypes.insert({structName, newType});
    bool added = TypeInfo::RegisterType(newType);
    if (!added)
    {
        delete newType;
        logger.LogError(*structDef->GetNameToken(), "Struct '{}' has already been defined", structName);
        return false;
    }

    return true;
}

const TypeInfo* SemanticAnalyzer::NameToType(const vector<const Token*>& typeNameTokens)
{
    size_t typeNameSize = typeNameTokens.size();
    if (typeNameSize == 0)
    {
        logger.LogInternalError("Empty type name");
        return nullptr;
    }

    size_t idx = typeNameSize;

    const Token* token = nullptr;
    size_t arrayLevel = 0;
    while (idx > 0)
    {
        --idx;

        token = typeNameTokens[idx];
        if (token->type == Token::eCloseBracket)
        {
            ++arrayLevel;

            if (idx == 0)
            {
                // TODO: better error message?
                logger.LogError(*token, "Invalid type");
                return nullptr;
            }
        }
        else
        {
            break;
        }
    }

    const Token* typeNameToken = typeNameTokens[idx];
    const string& name = typeNameToken->value;
    const TypeInfo* type = TypeInfo::GetType(name);
    if (type == nullptr)
    {
        logger.LogError(*typeNameToken, "'{}' is not a known type", name);
        return nullptr;
    }

    while (idx > 0)
    {
        --idx;

        token = typeNameTokens[idx];
        Token::EType tokenType = token->type;
        if (tokenType == Token::eAmpersand)
        {
            type = TypeInfo::GetPointerToType(type);
        }
        else if (tokenType == Token::eOpenBracket)
        {
            if (arrayLevel == 0)
            {
                logger.LogError(*token, "'[' does not have closing ']'");
                return nullptr;
            }

            --arrayLevel;
            type = TypeInfo::GetArrayOfType(type);
        }
        else
        {
            // TODO: better error message?
            logger.LogError(*token, "Unexpected token '{}'", token->value);
            return nullptr;
        }
    }

    // check if there is an extra closing brace
    if (arrayLevel > 0)
    {
        // find the extra brace
        size_t level = 0;
        const Token* braceToken = nullptr;
        for (size_t i = 0; i < typeNameSize; ++i)
        {
            const Token* t = typeNameTokens[i];
            Token::EType tokenType = t->type;
            if (tokenType == Token::eOpenBracket)
            {
                ++level;
            }
            else if (tokenType == Token::eCloseBracket)
            {
                if (level == 0)
                {
                    braceToken = t;
                    break;
                }

                --level;
            }
        }

        assert(braceToken != nullptr);
        logger.LogError(*braceToken, "Extra closing ']'");
        return nullptr;
    }

    return type;
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

    const vector<ExternFunctionDeclaration*>& externFuncDecls = moduleDefinition->GetExternFunctionDeclarations();
    const vector<FunctionDefinition*>& funcDefs = moduleDefinition->GetFunctionDefinitions();

    functions.clear();
    functions.reserve(externFuncDecls.size() + funcDefs.size());

    for (ExternFunctionDeclaration* externFunc : externFuncDecls)
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
            logger.LogError(*decl->GetNameToken(), "Function '{}' has already been defined", name);
            return;
        }
    }

    for (FunctionDefinition* funcDef : funcDefs)
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
            logger.LogError(*decl->GetNameToken(), "Function '{}' has already been defined", name);
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
        logger.LogInternalError("Could not get type for numeric literal");
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
    stringLiteralExpression->SetType(TypeInfo::GetStringType());
}

void SemanticAnalyzer::Visit(VariableExpression* variableExpression)
{
    const string& varName = variableExpression->GetName();
    const TypeInfo* varType = symbolTable.GetVariableType(varName);
    if (varType == nullptr)
    {
        logger.LogError(*variableExpression->GetToken(), "Variable '{}' is not declared in the current scope", varName);
        isError = true;
    }
    else
    {
        variableExpression->SetType(varType);
        variableExpression->SetIsStorage(!varType->IsImmutable());
    }
}

void SemanticAnalyzer::Visit(ArraySizeValueExpression* arrayExpression)
{
    Expression* sizeExpression = arrayExpression->sizeExpression;
    sizeExpression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* sizeType = sizeExpression->GetType();
    TypeInfo::ESign sign = sizeType->GetSign();

    NumericExpression* numExpression = dynamic_cast<NumericExpression*>(sizeExpression);
    if ( numExpression == nullptr || (sign != TypeInfo::eUnsigned && sign != TypeInfo::eContextDependent) )
    {
        isError = true;
        logger.LogError(*arrayExpression->startToken, "Invalid array size. Array sizes must be unsigned numeric literals");
        return;
    }

    Expression* valueExpression = arrayExpression->valueExpression;
    valueExpression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* type = valueExpression->GetType();
    const TypeInfo* arrayType = TypeInfo::GetArrayOfType(type);
    arrayExpression->SetType(arrayType);
}

void SemanticAnalyzer::Visit(ArrayMultiValueExpression* arrayExpression)
{
    const Expressions& expressions = arrayExpression->expressions;
    assert(expressions.size() > 0);

    Expression* expr = expressions[0];
    expr->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* type = expr->GetType();

    for (size_t i = 1; i < expressions.size(); ++i)
    {
        expr = expressions[i];
        expr->Accept(this);
        if (isError)
        {
            return;
        }

        // make sure all the types match
        const TypeInfo* exprType = expr->GetType();
        if (!exprType->IsSameAs(*type))
        {
            if (exprType->IsInt() && type->IsInt() && HaveCompatibleSigns(exprType, type))
            {
                type = GetBiggestSizeType(exprType, type);
            }
            else
            {
                isError = true;
                logger.LogError(*arrayExpression->startToken,
                    "Array item at index {} with type '{}' does not match previous item type '{}'",
                    i,
                    exprType->GetShortName(),
                    type->GetShortName()
                );
                return;
            }
        }
    }

    const TypeInfo* arrayType = TypeInfo::GetArrayOfType(type);
    arrayExpression->SetType(arrayType);
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
        logger.LogInternalError("Block expression has no sub-expressions");
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

void SemanticAnalyzer::Visit(CastExpression* castExpression)
{
    Expression* subExpression = castExpression->subExpression;
    subExpression->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* exprType = subExpression->GetType();

    const TypeInfo* castType = NameToType(castExpression->castTypeNameTokens);
    if (castType == nullptr)
    {
        isError = true;
        return;
    }

    // check if the cast is valid
    bool canCast = false;
    if (exprType->IsBool())
    {
        canCast = castType->IsBool() | castType->IsInt();
    }
    else if (exprType->IsInt())
    {
        canCast = castType->IsBool() | castType->IsInt();
    }

    if (!canCast)
    {
        logger.LogError(*castExpression->castToken, "Cannot cast expression of type '{}' to type '{}'",
            exprType->GetShortName(), castType->GetShortName());
        isError = true;
        return;
    }

    castExpression->SetType(castType);
}

void SemanticAnalyzer::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    auto iter = functions.find(funcName);
    if (iter == functions.cend())
    {
        logger.LogError(*functionExpression->GetNameToken(), "Function '{}' is not defined", funcName);
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
        logger.LogError(*functionExpression->GetNameToken(), "Function '{}' expected {} argument{} but got {}", funcName, params.size(), suffix, args.size());
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

        // check if we need to fix array literal type
        if (argType->IsArray() && paramType->IsArray())
        {
            bool ok = FixNumericLiteralType(arg, paramType);
            if (!ok)
            {
                isError = true;
                return;
            }

            // update argType in case it changed
            argType = arg->GetType();
        }

        if (!argType->IsSameAs(*paramType))
        {
            if ( !(argType->IsInt() && paramType->IsInt() && HaveCompatibleSigns(paramType, argType) && HaveCompatibleAssignmentSizes(paramType, argType)) )
            {
                logger.LogError(*functionExpression->GetNameToken(), "Argument type does not match parameter type. Argument: '{}', parameter: '{}'", argType->GetShortName(), paramType->GetShortName());
                isError = true;
                return;
            }
        }
    }

    // set the expression's function declaration
    functionExpression->SetFunctionDeclaration(funcDecl);

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

    const TypeInfo* type = expr->GetType();

    // if type is a pointer, get the type it points to
    if (type->IsPointer())
    {
        type = type->GetInnerType();
    }

    // check if member is available for this type
    const string& memberName = memberExpression->GetMemberName();
    const MemberInfo* member = type->GetMember(memberName);
    if (member == nullptr)
    {
        const Token* memberToken = memberExpression->GetMemberNameToken();
        logger.LogError(*memberToken, "Type '{}' has no member named '{}'", type->GetShortName(), memberName);
        isError = true;
        return;
    }

    memberExpression->SetType(member->GetType());
    memberExpression->SetIsStorage(expr->GetIsStorage() && member->GetIsStorage());
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
        logger.LogError(*branchExpression->ifToken, "If condition must be a boolean expression");
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
    else if (AreCompatibleRanges(ifType, elseType, resultType))
    {
        // resultType is assigned in else if function call
    }
    else
    {
        isError = true;
        logger.LogError(*branchExpression->ifToken, "'if' and 'else' expressions have mismatching types ('{}' and '{}')",
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
        logger.LogInternalError("Binary expression in variable declaration is not an assignment");
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

    // set the variable type
    const TypeInfo* type = GetVariableType(variableDeclaration->GetTypeNameTokens(), rightExpr->GetType());
    if (isError)
    {
        return;
    }
    variableDeclaration->SetVariableType(type);

    // add the variable name to the symbol table
    const string& varName = variableDeclaration->GetName();
    bool ok = symbolTable.AddVariable(varName, variableDeclaration->GetVariableType());
    if (!ok)
    {
        isError = true;
        logger.LogError(*variableDeclaration->GetNameToken(), "Variable '{}' has already been declared", varName);
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
            const Token* paramToken = param->GetNameToken();
            logger.LogError(*paramToken, "Function '{}' has multiple parameters named '{}'", functionDeclaration->GetName(), paramName);
            return false;
        }

        const TypeInfo* paramType = NameToType(param->GetTypeNameTokens());
        if (paramType == nullptr)
        {
            return false;
        }

        param->SetType(paramType);
        processedParams.insert(paramName);
    }

    // set return type
    const TypeInfo* returnType = nullptr;
    const vector<const Token*>& returnTypeName = functionDeclaration->GetReturnTypeNameTokens();
    size_t returnTypeNameSize = returnTypeName.size();
    if (returnTypeNameSize == 0)
    {
        returnType = TypeInfo::UnitType;
    }
    else
    {
        returnType = NameToType(functionDeclaration->GetReturnTypeNameTokens());
        if (returnType == nullptr)
        {
            return false;
        }
    }

    functionDeclaration->SetReturnType(returnType);

    // the subscript operator uses exit() if a bounds check fails,
    // so we need to make sure it has the right signature
    if (functionDeclaration->GetName() == "exit")
    {
        const vector<Parameter*>& params = functionDeclaration->GetParameters();
        if (params.size() != 1 || !params[0]->GetType()->IsSameAs(*TypeInfo::Int32Type))
        {
            logger.LogError(*functionDeclaration->GetNameToken(), "Function 'exit' must have exactly one parameter with type 'i32'");
            return false;
        }
    }
    // the subscript operator optionally uses logError() if a bounds check fails,
    // so we need to make sure it has the right signature
    else if (functionDeclaration->GetName() == "logError")
    {
        const vector<Parameter*>& params = functionDeclaration->GetParameters();
        if (params.size() != 3
        || !params[0]->GetType()->IsSameAs(*TypeInfo::GetStringType())
        || !params[1]->GetType()->IsSameAs(*TypeInfo::UInt32Type)
        || !params[2]->GetType()->IsSameAs(*TypeInfo::GetStringType()))
        {
            logger.LogError(*functionDeclaration->GetNameToken(), "Function 'logError' must have the following parameter types: 'str', 'u32', 'str'");
            return false;
        }
    }

    return true;
}

const TypeInfo* SemanticAnalyzer::InferType(const TypeInfo* inferType)
{
    const TypeInfo* type = inferType;

    // if this is an int literal, set the type to the minimum signed size
    // that will hold the number
    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(type);
    if (literalType != nullptr)
    {
        type = TypeInfo::GetMinSignedIntTypeForSize(literalType->GetSignedNumBits());
        if (type == nullptr)
        {
            isError = true;
            logger.LogInternalError("Could not infer integer literal type");
            return nullptr;
        }
    }
    else if (type->IsRange())
    {
        // if this is a range type and the members are int literals, set the members' type
        // to the minimum signed size that will hold the number
        const TypeInfo* memberType = type->GetInnerType();
        const NumericLiteralType* memberLiteralType = dynamic_cast<const NumericLiteralType*>(memberType);
        if (memberLiteralType != nullptr)
        {
            const TypeInfo* newMemberType = TypeInfo::GetMinSignedIntTypeForSize(memberLiteralType->GetSignedNumBits());
            if (newMemberType == nullptr)
            {
                isError = true;
                logger.LogInternalError("Could not infer Range integer literal type");
                return nullptr;
            }

            type = TypeInfo::GetRangeType(newMemberType, type->IsHalfOpen());
        }
    }
    else if (type->IsArray())
    {
        // if this is an array type and the items are int literals, set the inner type
        // to the minimum signed size that will hold the numbers
        const TypeInfo* innerType = type->GetInnerType();
        const TypeInfo* newInnerType = InferType(innerType);

        type = TypeInfo::GetArrayOfType(newInnerType);
    }

    return type;
}

const TypeInfo* SemanticAnalyzer::GetVariableType(const vector<const Token*>& typeNameTokens, const TypeInfo* inferType)
{
    bool inferTypeName = typeNameTokens.empty();
    const TypeInfo* type = nullptr;

    // if no type name was given, infer it from the expression
    if (inferTypeName)
    {
        type = InferType(inferType);
    }
    else // get the type from the name given
    {
        type = NameToType(typeNameTokens);
        if (type == nullptr)
        {
            isError = true;
            return nullptr;
        }
    }

    return type;
}

bool SemanticAnalyzer::CheckReturnType(const FunctionDeclaration* funcDecl, const Expression* expression, const Token* errorToken)
{
    const TypeInfo* returnType = funcDecl->GetReturnType();
    const TypeInfo* expressionType = expression->GetType();
    if (!returnType->IsSameAs(*expressionType))
    {
        if ( !(expressionType->IsInt() && returnType->IsInt() && HaveCompatibleSigns(returnType, expressionType) && HaveCompatibleAssignmentSizes(returnType, expressionType)) )
        {
            logger.LogError(*errorToken, "Function '{}' has an invalid return type. Expected '{}' but got '{}'", funcDecl->GetName(), returnType->GetShortName(), expressionType->GetShortName());
            return false;
        }
    }

    return true;
}
