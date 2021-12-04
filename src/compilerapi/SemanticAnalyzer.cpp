#include "CompilerContext.h"
#include "ErrorLogger.h"
#include "keywords.h"
#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include "utils.h"
#include <cassert>

using namespace std;
using namespace SyntaxTree;

SemanticAnalyzer::SemanticAnalyzer(CompilerContext& compilerContext) :
    logger(compilerContext.logger),
    isError(false),
    isConstDecl(false),
    compilerContext(compilerContext),
    symbolTable(compilerContext),
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
    Expression* subExpr = unaryExpression->subExpression;
    subExpr->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* subExprType = subExpr->GetType();
    bool isInt = subExprType->IsInt();

    UnaryExpression::EOperator op = unaryExpression->op;

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

                    resultType = NumericLiteralType::CreateSigned(subExprLiteralType->GetSignedNumBits());
                    ok = true;
                }
            }
            break;
        }

        case UnaryExpression::eComplement:
            ok = subExprType->IsSameAs(*TypeInfo::BoolType) || isInt;
            resultType = subExpr->GetType();
            break;

        case UnaryExpression::eAddressOf:
        {
            if (subExprType->IsType())
            {
                if (subExpr->GetIsConstant())
                {
                    const TypeInfo* subExprValue = compilerContext.GetTypeConstantValue(subExpr->GetConstantValueIndex());
                    const TypeInfo* exprValue = compilerContext.typeRegistry.GetPointerToType(subExprValue);
                    unsigned idx = compilerContext.AddTypeConstantValue(exprValue);
                    unaryExpression->SetConstantValueIndex(idx);

                    ok = true;
                    resultType = TypeInfo::TypeType;
                }
                // TODO: log specific error message if subExpr is not a constant
            }
            else
            {
                ok = subExpr->GetIsStorage();
                subExpr->SetAccessType(Expression::eAddress);
                resultType = compilerContext.typeRegistry.GetPointerToType(subExprType);
            }
            break;
        }

        case UnaryExpression::eDereference:
            ok = subExprType->IsPointer();
            resultType = subExprType->GetInnerType();
            unaryExpression->SetIsStorage(subExpr->GetIsStorage());
            break;

        case UnaryExpression::eArrayOf:
        {
            if (subExprType->IsType())
            {
                if (subExpr->GetIsConstant())
                {
                    const TypeInfo* subExprValue = compilerContext.GetTypeConstantValue(subExpr->GetConstantValueIndex());
                    const TypeInfo* exprValue = compilerContext.typeRegistry.GetArrayOfType(subExprValue);
                    unsigned idx = compilerContext.AddTypeConstantValue(exprValue);
                    unaryExpression->SetConstantValueIndex(idx);

                    ok = true;
                    resultType = TypeInfo::TypeType;
                }
                // TODO: log specific error message if subExpr is not a constant
            }
            break;
        }
    }

    if (!ok)
    {
        string opString = UnaryExpression::GetOperatorString(op);
        logger.LogError(*unaryExpression->opToken, "Unary operator '{}' does not support type '{}'", opString, subExprType->GetShortName());
        isError = true;
        return;
    }

    unaryExpression->SetType(resultType);

    if (subExpr->GetIsConstant())
    {
        if (subExprType->IsBool())
        {
            if (op == UnaryExpression::eComplement)
            {
                bool subValue = compilerContext.GetBoolConstantValue(subExpr->GetConstantValueIndex());
                unsigned idx = compilerContext.AddBoolConstantValue(!subValue);
                unaryExpression->SetConstantValueIndex(idx);
            }
        }
        else if (subExprType->IsInt())
        {
            int64_t subValue = compilerContext.GetIntConstantValue(subExpr->GetConstantValueIndex());

            bool isConst = true;
            int64_t value = 0;
            switch (op)
            {
                case UnaryExpression::eNegative:
                    value = -subValue;
                    break;
                case UnaryExpression::eComplement:
                    value = ~subValue;
                    break;
                default:
                    isConst = false;
                    break;
            }

            if (isConst)
            {
                unsigned idx = compilerContext.AddIntConstantValue(value);
                unaryExpression->SetConstantValueIndex(idx);
            }
        }
    }
}

void SemanticAnalyzer::Visit(BinaryExpression* binaryExpression)
{
    if (binaryExpression->left->GetType() == nullptr)
    {
        binaryExpression->left->Accept(this);
        if (isError)
        {
            return;
        }
    }

    if (binaryExpression->right->GetType() == nullptr)
    {
        binaryExpression->right->Accept(this);
        if (isError)
        {
            return;
        }
    }

    BinaryExpression::EOperator op = binaryExpression->op;
    bool isAssignment = BinaryExpression::IsAssignment(op);

    // implicit cast if necessary
    if (binaryExpression->left->GetType()->IsInt() && binaryExpression->right->GetType()->IsInt())
    {
        unsigned leftSize = binaryExpression->left->GetType()->GetNumBits();
        unsigned rightSize = binaryExpression->right->GetType()->GetNumBits();
        if (isAssignment)
        {
            if (binaryExpression->left->GetType()->GetSign() == binaryExpression->right->GetType()->GetSign() && leftSize > rightSize)
            {
                binaryExpression->right = ImplicitCast(binaryExpression->right, binaryExpression->left->GetType());
            }
        }
        else
        {
            if (binaryExpression->left->GetType()->GetSign() == binaryExpression->right->GetType()->GetSign())
            {
                if (leftSize < rightSize)
                {
                    binaryExpression->left = ImplicitCast(binaryExpression->left, binaryExpression->right->GetType());
                }
                else if (leftSize > rightSize)
                {
                    binaryExpression->right = ImplicitCast(binaryExpression->right, binaryExpression->left->GetType());
                }
            }
        }
    }

    bool ok = true;
    if (isAssignment)
    {
        if (!binaryExpression->left->GetIsStorage() && !binaryExpression->left->GetIsConstant())
        {
            logger.LogError(*binaryExpression->opToken, "Cannot assign to expression");
            isError = true;
            return;
        }
        else if (binaryExpression->left->GetIsConstant() && !isConstDecl)
        {
            logger.LogError(*binaryExpression->opToken, "Cannot assign to constant identifier");
            isError = true;
            return;
        }

        binaryExpression->left->SetAccessType(Expression::eAddress);

        // we need to check if the assignment types are compatible unless it's a shift
        bool needsCast = false;
        if (op != BinaryExpression::eShiftLeftAssign &&
            op != BinaryExpression::eShiftRightLogicalAssign &&
            op != BinaryExpression::eShiftRightArithmeticAssign
        )
        {
            ok = AreCompatibleAssignmentTypes(binaryExpression->left->GetType(), binaryExpression->right->GetType(), needsCast);
            if (needsCast)
            {
                binaryExpression->right = ImplicitCast(binaryExpression->right, binaryExpression->left->GetType());
            }
        }
    }

    if (ok)
    {
        ok = CheckBinaryOperatorTypes(binaryExpression);
    }

    if (!ok)
    {
        // TODO: Need better error message when integer is too big for assignment: var err u8 = 256;
        const Token* opToken = binaryExpression->opToken;
        string opString = BinaryExpression::GetOperatorString(op);
        logger.LogError(
            *opToken,
            "Binary operator '{}' does not support types '{}' and '{}'",
            opString,
            binaryExpression->left->GetType()->GetShortName(),
            binaryExpression->right->GetType()->GetShortName());

        isError = true;
        return;
    }

    const TypeInfo* resultType = GetBinaryOperatorResultType(op, binaryExpression->left->GetType(), binaryExpression->right->GetType());
    binaryExpression->SetType(resultType);

    if (op == BinaryExpression::eSubscript && binaryExpression->left->GetType()->IsArray() && binaryExpression->right->GetType()->IsInt())
    {
        binaryExpression->SetIsStorage(binaryExpression->left->GetIsStorage());
    }

    if (binaryExpression->left->GetIsConstant() && binaryExpression->right->GetIsConstant())
    {
        const TypeInfo* leftType = binaryExpression->left->GetType();
        const TypeInfo* rightType = binaryExpression->right->GetType();

        if (leftType->IsBool() && rightType->IsBool())
        {
            bool leftValue = compilerContext.GetBoolConstantValue(binaryExpression->left->GetConstantValueIndex());
            bool rightValue = compilerContext.GetBoolConstantValue(binaryExpression->right->GetConstantValueIndex());

            bool isConst = true;
            bool value = false;
            switch (op)
            {
                case BinaryExpression::eEqual:
                    value = leftValue == rightValue;
                    break;
                case BinaryExpression::eNotEqual:
                    value = leftValue != rightValue;
                    break;
                case BinaryExpression::eBitwiseAnd:
                    value = leftValue & rightValue;
                    break;
                case BinaryExpression::eBitwiseXor:
                    value = leftValue ^ rightValue;
                    break;
                case BinaryExpression::eBitwiseOr:
                    value = leftValue | rightValue;
                    break;
                default:
                    isConst = false;
                    break;
            }

            if (isConst)
            {
                unsigned idx = compilerContext.AddBoolConstantValue(value);
                binaryExpression->SetConstantValueIndex(idx);
            }
        }
        else if (leftType->IsInt() && rightType->IsInt())
        {
            int64_t leftValue = compilerContext.GetIntConstantValue(binaryExpression->left->GetConstantValueIndex());
            int64_t rightValue = compilerContext.GetIntConstantValue(binaryExpression->right->GetConstantValueIndex());

            bool isConst = true;
            bool boolValue = false;
            int64_t intValue = 0;
            RangeConstValue rangeValue;
            switch (op)
            {
                case BinaryExpression::eEqual:
                    boolValue = leftValue == rightValue;
                    break;
                case BinaryExpression::eNotEqual:
                    boolValue = leftValue != rightValue;
                    break;
                case BinaryExpression::eLessThan:
                    boolValue = leftValue < rightValue;
                    break;
                case BinaryExpression::eLessThanOrEqual:
                    boolValue = leftValue <= rightValue;
                    break;
                case BinaryExpression::eGreaterThan:
                    boolValue = leftValue > rightValue;
                    break;
                case BinaryExpression::eGreaterThanOrEqual:
                    boolValue = leftValue >= rightValue;
                    break;
                case BinaryExpression::eAdd:
                    intValue = leftValue + rightValue;
                    break;
                case BinaryExpression::eSubtract:
                    intValue = leftValue - rightValue;
                    break;
                case BinaryExpression::eMultiply:
                    intValue = leftValue * rightValue;
                    break;
                case BinaryExpression::eDivide:
                {
                    if (rightValue == 0)
                    {
                        const Token* opToken = binaryExpression->opToken;
                        logger.LogError(*opToken, "Divide by zero in constant expression");
                        isError = true;
                        return;
                    }
                    intValue = leftValue / rightValue;
                    break;
                }
                case BinaryExpression::eRemainder:
                {
                    if (rightValue == 0)
                    {
                        const Token* opToken = binaryExpression->opToken;
                        logger.LogError(*opToken, "Divide by zero in constant expression");
                        isError = true;
                        return;
                    }
                    intValue = leftValue % rightValue;
                    break;
                }
                case BinaryExpression::eShiftLeft:
                    intValue = leftValue << rightValue;
                    break;
                case BinaryExpression::eShiftRightLogical:
                {
                    int64_t mask = getBitMask(leftType->GetNumBits());
                    if (mask == 0)
                    {
                        logger.LogInternalError("Invalid int type size");
                        isError = true;
                        return;
                    }
                    intValue = static_cast<uint64_t>(leftValue & mask) >> rightValue;
                    break;
                }
                case BinaryExpression::eShiftRightArithmetic:
                    intValue = leftValue >> rightValue;
                    break;
                case BinaryExpression::eBitwiseAnd:
                    intValue = leftValue & rightValue;
                    break;
                case BinaryExpression::eBitwiseXor:
                    intValue = leftValue ^ rightValue;
                    break;
                case BinaryExpression::eBitwiseOr:
                    intValue = leftValue | rightValue;
                    break;
                case BinaryExpression::eClosedRange:
                case BinaryExpression::eHalfOpenRange:
                    rangeValue.start = leftValue;
                    rangeValue.end = rightValue;
                    break;
                default:
                    isConst = false;
                    break;
            }

            if (isConst)
            {
                unsigned idx = 0;
                if (resultType->IsBool())
                {
                    idx = compilerContext.AddBoolConstantValue(boolValue);
                }
                else if (resultType->IsInt())
                {
                    idx = compilerContext.AddIntConstantValue(intValue);
                }
                else if (resultType->IsRange())
                {
                    idx = compilerContext.AddRangeConstantValue(rangeValue);
                }

                binaryExpression->SetConstantValueIndex(idx);
            }
        }
        else if (leftType->IsStr() && op == BinaryExpression::eSubscript)
        {
            vector<char> strValue = compilerContext.GetStrConstantValue(binaryExpression->left->GetConstantValueIndex());
            size_t strSize = strValue.size();

            if (rightType->IsInt())
            {
                int64_t rightValue = compilerContext.GetIntConstantValue(binaryExpression->right->GetConstantValueIndex());
                uint64_t strIdx = static_cast<uint64_t>(rightValue);

                // check index
                if (strIdx >= strSize)
                {
                    logger.LogError(
                        *binaryExpression->opToken,
                        "Index out-of-bounds. Index: {}, string size: {}",
                        strIdx,
                        strSize
                    );
                    isError = true;
                    return;
                }

                uint8_t value = strValue[strIdx];
                unsigned idx = compilerContext.AddIntConstantValue(value);
                binaryExpression->SetConstantValueIndex(idx);
            }
            else if (rightType->IsRange())
            {
                const RangeConstValue& rightValue = compilerContext.GetRangeConstantValue(binaryExpression->right->GetConstantValueIndex());
                uint64_t startIdx = static_cast<uint64_t>(rightValue.start);
                uint64_t endIdx = static_cast<uint64_t>(rightValue.end);

                // if this range is closed, add 1 to the end
                if (!rightType->IsHalfOpen())
                {
                    endIdx += 1;
                }

                // check end
                if (endIdx > strSize)
                {
                    endIdx = strSize;
                }

                // check start
                if (startIdx > endIdx)
                {
                    startIdx = endIdx;
                }

                vector<char> newStrValue;
                for (uint64_t i = startIdx; i < endIdx; ++i)
                {
                    newStrValue.push_back(strValue[i]);
                }

                unsigned idx = compilerContext.AddStrConstantValue(newStrValue);
                binaryExpression->SetConstantValueIndex(idx);
            }
        }
        else if (leftType->IsArray() && op == BinaryExpression::eSubscript)
        {
            ArrayConstValue arrayValue = compilerContext.GetArrayConstantValue(binaryExpression->left->GetConstantValueIndex());
            size_t arraySize = compilerContext.GetArrayConstantValueSize(arrayValue);
            if (rightType->IsInt())
            {
                int64_t rightValue = compilerContext.GetIntConstantValue(binaryExpression->right->GetConstantValueIndex());
                uint64_t arrayIdx = static_cast<uint64_t>(rightValue);

                // check index
                if (arrayIdx >= arraySize)
                {
                    logger.LogError(
                        *binaryExpression->opToken,
                        "Index out-of-bounds. Index: {}, array size: {}",
                        arrayIdx,
                        arraySize
                    );
                    isError = true;
                    return;
                }

                unsigned valueIdx = 0;
                if (arrayValue.type == ArrayConstValue::eMultiValue)
                {
                    valueIdx = arrayValue.valueIndices[arrayIdx];
                }
                else
                {
                    valueIdx = arrayValue.valueIndices[1];
                }
                binaryExpression->SetConstantValueIndex(valueIdx);
            }
            else if (rightType->IsRange())
            {
                const RangeConstValue& rightValue = compilerContext.GetRangeConstantValue(binaryExpression->right->GetConstantValueIndex());
                uint64_t startIdx = static_cast<uint64_t>(rightValue.start);
                uint64_t endIdx = static_cast<uint64_t>(rightValue.end);

                // if this range is closed, add 1 to the end
                if (!rightType->IsHalfOpen())
                {
                    endIdx += 1;
                }

                // check end
                if (endIdx > arraySize)
                {
                    endIdx = arraySize;
                }

                // check start
                if (startIdx > endIdx)
                {
                    startIdx = endIdx;
                }

                ArrayConstValue newArrayValue;
                if (arrayValue.type == ArrayConstValue::eMultiValue)
                {
                    newArrayValue.type = ArrayConstValue::eMultiValue;
                    for (uint64_t i = startIdx; i < endIdx; ++i)
                    {
                        newArrayValue.valueIndices.push_back(arrayValue.valueIndices[i]);
                    }
                }
                else
                {
                    int64_t newSize = endIdx - startIdx;
                    unsigned newSizeIdx = compilerContext.AddIntConstantValue(newSize);

                    newArrayValue.type = ArrayConstValue::eSizeValue;
                    newArrayValue.valueIndices.push_back(newSizeIdx);
                    newArrayValue.valueIndices.push_back(arrayValue.valueIndices[1]);
                }

                unsigned idx = compilerContext.AddArrayConstantValue(newArrayValue);
                binaryExpression->SetConstantValueIndex(idx);
            }
        }
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

bool SemanticAnalyzer::AreCompatibleAssignmentTypes(const TypeInfo* assignType, const TypeInfo* exprType, bool& needsCast)
{
    if (assignType->IsSameAs(*exprType))
    {
        needsCast = false;
        return true;
    }

    if (assignType->IsInt() && exprType->IsInt()
        && HaveCompatibleSigns(assignType, exprType)
        && HaveCompatibleAssignmentSizes(assignType, exprType))
    {
        needsCast = true;
        return true;
    }

    const TypeInfo* assignInnerType = assignType;
    const TypeInfo* exprInnerType = exprType;
    while (assignInnerType->IsArray() && exprInnerType->IsArray())
    {
        assignInnerType = assignInnerType->GetInnerType();
        exprInnerType = exprInnerType->GetInnerType();
    }

    if (assignInnerType->IsRange() && exprInnerType->IsRange())
    {
        assignInnerType = assignInnerType->GetInnerType();
        exprInnerType = exprInnerType->GetInnerType();
    }

    if (assignInnerType->IsInt() && exprInnerType->IsNumericLiteral()
        && HaveCompatibleSigns(assignInnerType, exprInnerType)
        && HaveCompatibleAssignmentSizes(assignInnerType, exprInnerType))
    {
        needsCast = true;
        return true;
    }

    needsCast = false;
    return false;
}

const NumericLiteralType* SemanticAnalyzer::GetBiggestNumLitSizeType(const NumericLiteralType* type1, const NumericLiteralType* type2)
{
    const NumericLiteralType* resultType = nullptr;

    if (type1->GetSign() == TypeInfo::eSigned || type2->GetSign() == TypeInfo::eSigned)
    {
        unsigned signedNumBits = max(type1->GetSignedNumBits(), type2->GetSignedNumBits());
        resultType = NumericLiteralType::CreateSigned(signedNumBits);
    }
    else
    {
        if (type1->GetSignedNumBits() >= type2->GetSignedNumBits())
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

const TypeInfo* SemanticAnalyzer::GetBiggestSizeType(const NumericLiteralType* type1, const TypeInfo* type2, unsigned& type1Size, unsigned& type2Size)
{
    const TypeInfo* resultType = nullptr;
    type2Size = type2->GetNumBits();
    TypeInfo::ESign type2Sign = type2->GetSign();

    if (type2Sign == TypeInfo::eSigned)
    {
        type1Size = type1->GetSignedNumBits();
        unsigned numBits = (type1Size > type2Size) ? type1Size : type2Size;
        resultType = TypeInfo::GetMinSignedIntTypeForSize(numBits);
    }
    else
    {
        type1Size = type1->GetUnsignedNumBits();
        unsigned numBits = (type1Size > type2Size) ? type1Size : type2Size;
        resultType = TypeInfo::GetMinUnsignedIntTypeForSize(numBits);
    }

    return resultType;
}

const TypeInfo* SemanticAnalyzer::GetBiggestSizeType(const TypeInfo* type1, const TypeInfo* type2)
{
    const TypeInfo* resultType = nullptr;
    const NumericLiteralType* literalType1 = dynamic_cast<const NumericLiteralType*>(type1);
    const NumericLiteralType* literalType2 = dynamic_cast<const NumericLiteralType*>(type2);

    if (literalType1 != nullptr && literalType2 != nullptr)
    {
        resultType = GetBiggestNumLitSizeType(literalType1, literalType2);
    }
    else if (literalType1 != nullptr)
    {
        unsigned type1Size, type2Size;
        resultType = GetBiggestSizeType(literalType1, type2, type1Size, type2Size);
    }
    else if (literalType2 != nullptr)
    {
        unsigned type1Size, type2Size;
        resultType = GetBiggestSizeType(literalType2, type1, type1Size, type2Size);
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

void SemanticAnalyzer::FixNumericLiteralExpression(Expression* expr, const TypeInfo* resultType)
{
    if (NumericExpression* numExpr = dynamic_cast<NumericExpression*>(expr); numExpr != nullptr)
    {
        numExpr->SetType(resultType);
    }
    else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr); unaryExpr != nullptr)
    {
        FixNumericLiteralExpression(unaryExpr->subExpression, resultType);
        unaryExpr->SetType(resultType);
    }
    else if (BinaryExpression* binExpr = dynamic_cast<BinaryExpression*>(expr); binExpr != nullptr)
    {
        BinaryExpression::EOperator op = binExpr->op;

        if (op == BinaryExpression::eClosedRange || op == BinaryExpression::eHalfOpenRange)
        {
            assert(resultType->IsRange() && "Expected result type to be a range");
            const TypeInfo* newSubType = resultType->GetInnerType();

            FixNumericLiteralExpression(binExpr->left, newSubType);
            FixNumericLiteralExpression(binExpr->right, newSubType);
        }
        else if (op == BinaryExpression::eShiftLeft || op == BinaryExpression::eShiftRightLogical || op == BinaryExpression::eShiftRightArithmetic)
        {
            FixNumericLiteralExpression(binExpr->left, resultType);
            if (binExpr->right->GetType()->GetSign() == TypeInfo::eContextDependent)
            {
                FixNumericLiteralExpression(binExpr->right, TypeInfo::GetMinUnsignedIntTypeForSize(resultType->GetNumBits()));
            }
        }
        else if (op == BinaryExpression::eSubscript)
        {
            const TypeInfo* leftType = binExpr->left->GetType();
            const TypeInfo* rightType = binExpr->right->GetType();

            // fix left expression
            if (leftType->IsStr())
            {
                // nothing to do; the string should already have the correct type
            }
            else if (leftType->IsArray())
            {
                const TypeInfo* newType = nullptr;
                if (rightType->IsInt())
                {
                    newType = compilerContext.typeRegistry.GetArrayOfType(resultType);
                }
                else if (rightType->IsRange())
                {
                    newType = resultType;
                }
                else
                {
                    assert(false && "Unexpected right type for subscript");
                }

                FixNumericLiteralExpression(binExpr->left, newType);
            }
            else
            {
                assert(false && "Unexpected left type for subscript");
            }

            // fix right expression
            if (rightType->IsInt())
            {
                FixNumericLiteralExpression(binExpr->right, compilerContext.typeRegistry.GetUIntSizeType());
            }
            else if (rightType->IsRange())
            {
                const TypeInfo* newType = compilerContext.typeRegistry.GetRangeType(compilerContext.typeRegistry.GetUIntSizeType(), rightType->IsHalfOpen());
                FixNumericLiteralExpression(binExpr->right, newType);
            }
            else
            {
                assert(false && "Unexpected right type for subscript");
            }
        }
        else
        {
            FixNumericLiteralExpression(binExpr->left, resultType);
            FixNumericLiteralExpression(binExpr->right, resultType);
        }

        binExpr->SetType(resultType);
    }
    else if (BlockExpression* blockExpr = dynamic_cast<BlockExpression*>(expr); blockExpr != nullptr)
    {
        Expression* lastExpression = dynamic_cast<Expression*>(blockExpr->statements.back());
        FixNumericLiteralExpression(lastExpression, resultType);
        blockExpr->SetType(resultType);
    }
    else if (BranchExpression* branchExpr = dynamic_cast<BranchExpression*>(expr); branchExpr != nullptr)
    {
        FixNumericLiteralExpression(branchExpr->ifExpression, resultType);
        FixNumericLiteralExpression(branchExpr->elseExpression, resultType);
        branchExpr->SetType(resultType);
    }
    else if (MemberExpression* memberExpr = dynamic_cast<MemberExpression*>(expr); memberExpr != nullptr)
    {
        const TypeInfo* currentSubExprType = memberExpr->subExpression->GetType();
        const TypeInfo* newSubExprType = resultType;
        if (currentSubExprType->IsRange())
        {
            newSubExprType = compilerContext.typeRegistry.GetRangeType(resultType, currentSubExprType->IsHalfOpen());
        }

        FixNumericLiteralExpression(memberExpr->subExpression, newSubExprType);
        memberExpr->SetType(resultType);
    }
    else if (ImplicitCastExpression* impCastExpr = dynamic_cast<ImplicitCastExpression*>(expr); impCastExpr != nullptr)
    {
        // we shouldn't need to recurse below an implicit cast
        // because the type should already be known

        impCastExpr->SetType(resultType);
    }
    else if (ArraySizeValueExpression* arrSizeValExpr = dynamic_cast<ArraySizeValueExpression*>(expr); arrSizeValExpr != nullptr)
    {
        const TypeInfo* innerType = resultType->GetInnerType();
        FixNumericLiteralExpression(arrSizeValExpr->valueExpression, innerType);
        arrSizeValExpr->SetType(resultType);
    }
    else if (ArrayMultiValueExpression* arrMultiValExpr = dynamic_cast<ArrayMultiValueExpression*>(expr); arrMultiValExpr != nullptr)
    {
        const TypeInfo* innerType = resultType->GetInnerType();
        for (Expression* valExpr : arrMultiValExpr->expressions)
        {
            FixNumericLiteralExpression(valExpr, innerType);
        }

        arrMultiValExpr->SetType(resultType);
    }
    else
    {
        assert(false && "Unexpected expression type in FixNumericLiteralExpression()");
    }
}

bool SemanticAnalyzer::CheckBinaryOperatorTypes(BinaryExpression* binExpr)
{
    bool ok = false;
    BinaryExpression::EOperator op = binExpr->op;
    Expression* left = binExpr->left;
    Expression* right = binExpr->right;
    const TypeInfo* leftType = left->GetType();
    const TypeInfo* rightType = right->GetType();

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
            {
                // require right operand of shift to be unsigned
                TypeInfo::ESign rightSign = rightType->GetSign();
                if (rightSign == TypeInfo::eUnsigned || rightSign == TypeInfo::eContextDependent)
                {
                    ok = true;
                }
                else
                {
                    ok = false;
                }
                break;
            }
            default:
                ok = false;
                break;
        }

        if (ok)
        {
            const NumericLiteralType* leftNumLit = dynamic_cast<const NumericLiteralType*>(left->GetType());
            const NumericLiteralType* rightNumLit = dynamic_cast<const NumericLiteralType*>(right->GetType());
            if (leftNumLit != nullptr && rightNumLit == nullptr)
            {
                // if the left operand is an integer literal and the operator is a shift, then we
                // don't have enough information yet to be able to determine the int literal's type
                if (op != BinaryExpression::eShiftLeft && op != BinaryExpression::eShiftRightArithmetic && op != BinaryExpression::eShiftRightLogical)
                {
                    const TypeInfo* resultType = GetBiggestSizeType(left->GetType(), right->GetType());
                    FixNumericLiteralExpression(left, resultType);
                    if (right->GetType()->GetNumBits() != resultType->GetNumBits())
                    {
                        binExpr->right = ImplicitCast(right, resultType);
                    }
                }
            }
            else if (leftNumLit == nullptr && rightNumLit != nullptr)
            {
                if (op == BinaryExpression::eShiftLeft || op == BinaryExpression::eShiftLeftAssign
                || op == BinaryExpression::eShiftRightArithmetic || op == BinaryExpression::eShiftRightArithmeticAssign
                || op == BinaryExpression::eShiftRightLogical || op == BinaryExpression::eShiftRightLogicalAssign)
                {
                    const TypeInfo* newType = TypeInfo::GetMinUnsignedIntTypeForSize(rightType->GetNumBits());
                    FixNumericLiteralExpression(right, newType);
                }
                else
                {
                    const TypeInfo* resultType = GetBiggestSizeType(left->GetType(), right->GetType());
                    FixNumericLiteralExpression(right, resultType);
                    if (left->GetType()->GetNumBits() != resultType->GetNumBits())
                    {
                        binExpr->left = ImplicitCast(left, resultType);
                    }
                }
            }
        }
    }
    else if (leftType->IsPointer() && rightType->IsPointer())
    {
        ok =
            (op == BinaryExpression::eAssign || op == BinaryExpression::eEqual || op == BinaryExpression::eNotEqual)
            && leftType->IsSameAs(*rightType);
    }
    else if (leftType->IsStr())
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
                right->SetType(newType);

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
                const TypeInfo* newType = compilerContext.typeRegistry.GetRangeType(newInnerType, rightType->IsHalfOpen());
                right->SetType(newType);

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
                right->SetType(newType);

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
                const TypeInfo* newType = compilerContext.typeRegistry.GetRangeType(newInnerType, rightType->IsHalfOpen());
                right->SetType(newType);

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
            return compilerContext.typeRegistry.GetRangeType(memberType, op == BinaryExpression::eHalfOpenRange);
        }
        case BinaryExpression::eSubscript:
        {
            if (rightType->IsInt())
            {
                if (leftType->IsStr())
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

    logger.LogInternalError("Operator type was not handled in switch statement");
    return nullptr;
}

void SemanticAnalyzer::Visit(WhileLoop* whileLoop)
{
    Expression* condition = whileLoop->condition;
    condition->Accept(this);
    if (isError)
    {
        return;
    }

    // ensure condition is a boolean expression
    if (!condition->GetType()->IsBool())
    {
        isError = true;
        logger.LogError(*whileLoop->whileToken, "While loop condition must be a boolean expression");
        return;
    }

    // check statements
    BlockExpression* expression = whileLoop->expression;
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
        const Token* endBlockToken = expression->endToken;
        logger.LogError(*endBlockToken, "While loop block expression must return the unit type");
        return;
    }
}

void SemanticAnalyzer::Visit(ForLoop* forLoop)
{
    // process iterable expression
    Expression* iterExpression = forLoop->iterExpression;
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
        logger.LogError(*forLoop->forToken, "For loop expression is not iterable");
        return;
    }

    // set variable type
    const TypeInfo* varType = GetVariableType(forLoop->varTypeExpression, inferType, forLoop->variableNameToken);
    if (isError)
    {
        return;
    }
    forLoop->variableType = varType;

    // set index type if there is an index variable
    const TypeInfo* uIntSizeType = compilerContext.typeRegistry.GetUIntSizeType();
    const TypeInfo* indexVarType = GetVariableType(forLoop->indexTypeExpression, uIntSizeType, forLoop->indexNameToken);
    if (isError)
    {
        return;
    }
    else if (!indexVarType->IsInt() || indexVarType->GetSign() != TypeInfo::eUnsigned || indexVarType->GetNumBits() < uIntSizeType->GetNumBits())
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

            const TypeInfo* newType = compilerContext.typeRegistry.GetArrayOfType(inferType);
            iterExpression->SetType(newType);
        }
    }

    // create new scope for variable
    Scope scope(symbolTable);

    // add the variable name to the symbol table
    const string& varName = forLoop->variableName;
    bool ok = symbolTable.AddVariable(varName, forLoop->variableNameToken, forLoop->variableType);
    if (!ok)
    {
        isError = true;
        LogExistingIdentifierError(varName, forLoop->variableNameToken);
        return;
    }

    // add the index variable name to the symbol table
    const string& indexVarName = forLoop->indexName;
    if (!indexVarName.empty())
    {
        ok = symbolTable.AddVariable(indexVarName, forLoop->indexNameToken, forLoop->indexType);
        if (!ok)
        {
            isError = true;
            LogExistingIdentifierError(indexVarName, forLoop->variableNameToken);
            return;
        }
    }

    // check variable type and iterable type
    if (!varType->IsSameAs(*inferType) && (!HaveCompatibleSigns(varType, inferType) || !HaveCompatibleAssignmentSizes(varType, inferType)))
    {
        isError = true;
        logger.LogError(
            *forLoop->variableNameToken,
            "Cannot assign value of type '{}' to variable '{}' of type '{}'",
            inferType->GetShortName(),
            varName,
            varType->GetShortName()
        );
        return;
    }

    // process body expression
    BlockExpression* expression = forLoop->expression;
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
        const Token* endBlockToken = expression->endToken;
        logger.LogError(*endBlockToken, "For loop block expression must return the unit type");
        return;
    }
}

void SemanticAnalyzer::Visit(LoopControl* loopControl)
{
    // make sure we're in a loop
    if (loopLevel == 0)
    {
        isError = true;
        const Token* token = loopControl->token;
        logger.LogError(*token, "'{}' can only be used in a loop", token->value);
        return;
    }
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

    // check if expression type matches function return type
    Expression* resultExpression = nullptr;
    bool ok = CheckReturnType(currentFunction->declaration, expression, ret->token, resultExpression);
    if (!ok)
    {
        isError = true;
        return;
    }

    if (resultExpression != nullptr)
    {
        ret->expression = resultExpression;
    }
}

void SemanticAnalyzer::Visit(FunctionTypeExpression* functionTypeExpression)
{
    functionTypeExpression->SetType(TypeInfo::TypeType);

    // process param types
    vector<const TypeInfo*> paramTypes;
    size_t numParams = functionTypeExpression->paramTypeExpressions.size();
    for (size_t i = 0; i < numParams; ++i)
    {
        Expression* paramTypeExpr = functionTypeExpression->paramTypeExpressions[i];
        paramTypeExpr->Accept(this);
        if (isError)
        {
            return;
        }

        const TypeInfo* paramType = TypeExpressionToType(paramTypeExpr, functionTypeExpression->paramNameTokens[i]);
        if (paramType == nullptr)
        {
            isError = true;
            return;
        }

        paramTypes.push_back(paramType);
    }

    // process return type
    const TypeInfo* returnType = nullptr;
    Expression* returnTypeExpr = functionTypeExpression->returnTypeExpression;
    if (returnTypeExpr == nullptr)
    {
        returnType = TypeInfo::UnitType;
    }
    else
    {
        returnTypeExpr->Accept(this);
        if (isError)
        {
            return;
        }

        returnType = TypeExpressionToType(functionTypeExpression->returnTypeExpression, functionTypeExpression->funToken);
        if (returnType == nullptr)
        {
            isError = true;
            return;
        }
    }

    const TypeInfo* funType =
        compilerContext.typeRegistry.GetFunctionType(
            paramTypes,
            functionTypeExpression->paramNames,
            returnType
        );
    unsigned idx = compilerContext.AddTypeConstantValue(funType);
    functionTypeExpression->SetConstantValueIndex(idx);
}

void SemanticAnalyzer::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    const FunctionDeclaration* funcDecl = externFunctionDeclaration->declaration;

    const TypeInfo* type = compilerContext.typeRegistry.GetFunctionType(funcDecl);
    externFunctionDeclaration->SetType(type);
}

void SemanticAnalyzer::Visit(FunctionDefinition* functionDefinition)
{
    const FunctionDeclaration* funcDecl = functionDefinition->declaration;

    const TypeInfo* type = compilerContext.typeRegistry.GetFunctionType(funcDecl);
    functionDefinition->SetType(type);

    // create new scope for parameters and add them
    Scope scope(symbolTable);

    for (const Parameter* param : funcDecl->parameters)
    {
        const string& paramName = param->name;
        const Token* paramToken = param->nameToken;
        bool ok = symbolTable.AddVariable(paramName, paramToken, param->type);
        if (!ok)
        {
            isError = true;
            LogExistingIdentifierError(paramName, paramToken);
            return;
        }
    }

    Expression* expression = functionDefinition->expression;
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
    SyntaxTreeNode* lastStatement = blockExpr->statements.back();
    Return* ret = dynamic_cast<Return*>(lastStatement);
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
        Expression* resultExpression = nullptr;
        bool ok = CheckReturnType(funcDecl, expression, blockExpr->endToken, resultExpression);
        if (!ok)
        {
            isError = true;
            return;
        }

        if (resultExpression != nullptr)
        {
            functionDefinition->expression = resultExpression;
        }
    }
}

void SemanticAnalyzer::Visit(StructDefinition* structDefinition)
{
    const string& structName = structDefinition->name;

    auto iter = partialStructTypes.find(structName);
    assert(iter != partialStructTypes.cend());
    AggregateType* newType = iter->second;

    for (const MemberDefinition* member : structDefinition->members)
    {
        const TypeInfo* memberType = TypeExpressionToType(member->typeExpression, member->nameToken);
        if (memberType == nullptr)
        {
            delete newType;
            isError = true;
            return;
        }

        const string& memberName = member->name;
        bool added = newType->AddMember(memberName, memberType, true, member->nameToken);
        if (!added)
        {
            delete newType;
            isError = true;
            logger.LogError(*member->nameToken, "Duplicate member '{}' in struct '{}'", memberName, structName);
            return;
        }
    }

    structDefinition->type = newType;
}

void SemanticAnalyzer::Visit(StructInitializationExpression* structInitializationExpression)
{
    const string& structName = structInitializationExpression->structName;
    const TypeInfo* type = compilerContext.typeRegistry.GetType(structName);
    if (type == nullptr)
    {
        isError = true;
        const Token* token = structInitializationExpression->structNameToken;
        logger.LogError(*token, "'{}' is not a known type", structName);
        return;
    }

    structInitializationExpression->SetType(type);

    unordered_set<string> membersToInit;
    for (const MemberInfo* member : type->GetMembers())
    {
        membersToInit.insert(member->GetName());
    }

    bool allMembersAreConst = true;
    for (MemberInitialization* member : structInitializationExpression->memberInitializations)
    {
        const string& memberName = member->name;

        // get member info
        const MemberInfo* memberInfo = type->GetMember(memberName);
        if (memberInfo == nullptr)
        {
            isError = true;
            const Token* token = member->nameToken;
            logger.LogError(*token, "Struct '{}' does not have a member named '{}'", structName, memberName);
            return;
        }

        size_t num = membersToInit.erase(memberName);
        if (num == 0)
        {
            isError = true;
            const Token* token = member->nameToken;
            logger.LogError(*token, "Member '{}' has already been initialized", memberName);
            return;
        }

        // evaluate expression
        Expression* expr = member->expression;
        expr->Accept(this);
        if (isError)
        {
            return;
        }
        allMembersAreConst &= expr->GetIsConstant();

        // check types
        const TypeInfo* memberType = memberInfo->GetType();
        const TypeInfo* exprType = expr->GetType();
        bool needsCast = false;
        if (!AreCompatibleAssignmentTypes(memberType, exprType, needsCast))
        {
            isError = true;
            const Token* token = member->nameToken;
            logger.LogError(*token, "Cannot assign expression of type '{}' to member '{}' of type '{}'",
                            exprType->GetShortName(), memberName, memberType->GetShortName());
            return;
        }

        if (needsCast)
        {
            member->expression = ImplicitCast(expr, memberType);
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
        const Token* token = structInitializationExpression->structNameToken;
        logger.LogError(*token, errorMsg.c_str());
        return;
    }

    if (allMembersAreConst)
    {
        StructConstValue constValue;
        for (const MemberInitialization* member : structInitializationExpression->memberInitializations)
        {
            constValue.memberIndices.push_back(member->expression->GetConstantValueIndex());
        }

        unsigned idx = compilerContext.AddStructConstantValue(constValue);
        structInitializationExpression->SetConstantValueIndex(idx);
    }
}

void SemanticAnalyzer::LogExistingIdentifierError(const string& name, const Token* token)
{
    logger.LogError(*token, "Identifier '{}' has already been declared", name);

    const SymbolTable::IdentifierData* data = symbolTable.GetIdentifierData(name);
    const Token* existingToken = data->token;
    if (existingToken != nullptr)
    {
        logger.LogNote(*existingToken, "Identifier is declared here");
    }
}

bool SemanticAnalyzer::SortTypeDefinitions(Modules* modules)
{
    vector<StructDefinition*> structDefs;
    for (ModuleDefinition* modDef : modules->modules)
    {
        for (StructDefinition* structDef : modDef->structDefinitions)
        {
            structDefs.push_back(structDef);
        }
    }
    size_t numStructDefs = structDefs.size();

    unordered_map<string, StructDefinition*> nameMap;
    nameMap.reserve(numStructDefs);

    // build map for fast lookup
    for (StructDefinition* structDef : structDefs)
    {
        const string& structName = structDef->name;
        auto rv = nameMap.insert({structName, structDef});
        if (!rv.second)
        {
            logger.LogError(*structDef->nameToken, "Struct '{}' has already been defined", structName);
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
        const string& structName = structDef->name;

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

    modules->orderedStructDefinitions.swap(ordered);

    return true;
}

bool SemanticAnalyzer::ResolveDependencies(
    StructDefinition* structDef,
    const unordered_map<string, StructDefinition*>& nameMap,
    vector<StructDefinition*>& ordered,
    unordered_set<string>& resolved,
    unordered_set<string>& dependents)
{
    const string& structName = structDef->name;

    for (const MemberDefinition* member : structDef->members)
    {
        IdentifierExpression* typeExpr = dynamic_cast<IdentifierExpression*>(member->typeExpression);

        // we only need to check types that might be structs
        // (which will be an IdentifierExpression)
        if (typeExpr != nullptr)
        {
            const string& memberTypeName = typeExpr->name;

            // if we have not seen this member's type yet, resolve its dependencies
            if (compilerContext.typeRegistry.GetType(memberTypeName) == nullptr && resolved.find(memberTypeName) == resolved.end())
            {
                // check for a recursive dependency
                auto dependentsIter = dependents.find(memberTypeName);
                if (dependentsIter != dependents.end())
                {
                    const string& memberName = member->name;
                    logger.LogError(*member->nameToken, "In struct '{}', member '{}' with type '{}' creates recursive dependency", structName, memberName, memberTypeName);
                    return false;
                }

                dependents.insert(structName);

                auto nameMapIter = nameMap.find(memberTypeName);
                if (nameMapIter == nameMap.end())
                {
                    logger.LogError(*typeExpr->token, "'{}' is not a known type", memberTypeName);
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
    AggregateType* newType = new AggregateType(structName, structDef->nameToken);
    partialStructTypes.insert({structName, newType});

    // TODO: Is this needed?
    bool added = compilerContext.typeRegistry.RegisterType(newType);
    if (!added)
    {
        delete newType;
        logger.LogError(*structDef->nameToken, "Struct '{}' has already been defined", structName);
        return false;
    }

    unsigned idx = compilerContext.AddTypeConstantValue(newType);
    bool ok = symbolTable.AddConstant(structName, structDef->nameToken, TypeInfo::TypeType, idx);
    if (!ok)
    {
        delete newType;
        LogExistingIdentifierError(structName, structDef->nameToken);
        return false;
    }

    return true;
}

const TypeInfo* SemanticAnalyzer::TypeExpressionToType(Expression* typeExpression, const Token* errorToken)
{
    typeExpression->Accept(this);
    if (isError)
    {
        return nullptr;
    }

    if (!typeExpression->GetType()->IsType())
    {
        // TODO: need a way to get the type expression tokens
        logger.LogError(*errorToken, "Expected expression to be of type '{}'", TYPE_KEYWORD);
        return nullptr;
    }
    if (!typeExpression->GetIsConstant())
    {
        // TODO: need a way to get the type expression tokens
        logger.LogError(*errorToken, "Type must be a constant expression");
        return nullptr;
    }

    unsigned idx = typeExpression->GetConstantValueIndex();
    const TypeInfo* type = compilerContext.GetTypeConstantValue(idx);
    return type;
}

void SemanticAnalyzer::Visit(ModuleDefinition* moduleDefinition)
{
    // perform semantic analysis on all functions
    for (FunctionDefinition* funcDef : moduleDefinition->functionDefinitions)
    {
        funcDef->Accept(this);
        if (isError)
        {
            return;
        }
    }
}

void SemanticAnalyzer::Visit(Modules* modules)
{
    // process constants
    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        const vector<ConstantDeclaration*>& constantDeclarations = moduleDefinition->constantDeclarations;

        for (ConstantDeclaration* constDecl : constantDeclarations)
        {
            constDecl->Accept(this);
            if (isError)
            {
                return;
            }
        }
    }

    // sort struct definitions so each comes after any struct definitions it depends on
    bool ok = SortTypeDefinitions(modules);
    if (!ok)
    {
        isError = true;
        return;
    }

    // process struct definitions
    for (StructDefinition* structDef : modules->orderedStructDefinitions)
    {
        structDef->Accept(this);
        if (isError)
        {
            return;
        }
    }

    // build a look-up table for all functions

    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        const vector<ExternFunctionDeclaration*>& externFuncDecls = moduleDefinition->externFunctionDeclarations;
        const vector<FunctionDefinition*>& funcDefs = moduleDefinition->functionDefinitions;

        for (ExternFunctionDeclaration* externFunc : externFuncDecls)
        {
            FunctionDeclaration* decl = externFunc->declaration;

            ok = SetFunctionDeclarationTypes(decl);
            if (!ok)
            {
                isError = true;
                return;
            }

            const TypeInfo* funType = compilerContext.typeRegistry.GetFunctionType(decl);
            unsigned idx = compilerContext.AddFunctionConstantValue(decl);
            externFunc->SetConstantValueIndex(idx);
            bool ok = symbolTable.AddConstant(decl->name, decl->nameToken, funType, idx);
            if (!ok)
            {
                isError = true;
                LogExistingIdentifierError(decl->name, decl->nameToken);
                return;
            }
        }

        for (FunctionDefinition* funcDef : funcDefs)
        {
            FunctionDeclaration* decl = funcDef->declaration;

            ok = SetFunctionDeclarationTypes(decl);
            if (!ok)
            {
                isError = true;
                return;
            }

            const TypeInfo* funType = compilerContext.typeRegistry.GetFunctionType(decl);
            unsigned idx = compilerContext.AddFunctionConstantValue(decl);
            funcDef->SetConstantValueIndex(idx);
            bool ok = symbolTable.AddConstant(decl->name, decl->nameToken, funType, idx);
            if (!ok)
            {
                isError = true;
                LogExistingIdentifierError(decl->name, decl->nameToken);
                return;
            }
        }
    }

    for (ModuleDefinition* module : modules->modules)
    {
        module->Accept(this);
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

    int64_t value = numericExpression->value;
    unsigned idx = compilerContext.AddIntConstantValue(value);
    numericExpression->SetConstantValueIndex(idx);
}

void SemanticAnalyzer::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    boolLiteralExpression->SetType(TypeInfo::BoolType);

    bool value = boolLiteralExpression->token->type == Token::eTrueLit;
    unsigned idx = compilerContext.AddBoolConstantValue(value);
    boolLiteralExpression->SetConstantValueIndex(idx);
}

void SemanticAnalyzer::Visit(StringLiteralExpression* stringLiteralExpression)
{
    stringLiteralExpression->SetType(compilerContext.typeRegistry.GetStringType());

    const vector<char>& value = stringLiteralExpression->characters;
    unsigned idx = compilerContext.AddStrConstantValue(value);
    stringLiteralExpression->SetConstantValueIndex(idx);
}

void SemanticAnalyzer::Visit(IdentifierExpression* identifierExpression)
{
    const string& name = identifierExpression->name;
    const SymbolTable::IdentifierData* data = symbolTable.GetIdentifierData(name);
    if (data == nullptr)
    {
        logger.LogError(*identifierExpression->token, "Identifier '{}' is not declared in the current scope", name);
        isError = true;
    }
    else
    {
        bool isConst = data->IsConstant();
        const TypeInfo* type = data->type;
        identifierExpression->SetType(type);
        identifierExpression->SetIsStorage(!isConst);

        if (isConst)
        {
            unsigned constIdx = data->constValueIndex;
            identifierExpression->SetConstantValueIndex(constIdx);
        }
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

    if ( !sizeExpression->GetIsConstant() || (sign != TypeInfo::eUnsigned && sign != TypeInfo::eContextDependent) )
    {
        isError = true;
        logger.LogError(*arrayExpression->startToken, "Invalid array size. Array sizes must be unsigned constant expressions");
        return;
    }

    Expression* valueExpression = arrayExpression->valueExpression;
    valueExpression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* type = valueExpression->GetType();
    const TypeInfo* arrayType = compilerContext.typeRegistry.GetArrayOfType(type);
    arrayExpression->SetType(arrayType);

    if (valueExpression->GetIsConstant())
    {
        ArrayConstValue constValue;
        constValue.type = ArrayConstValue::eSizeValue;
        constValue.valueIndices.push_back(sizeExpression->GetConstantValueIndex());
        constValue.valueIndices.push_back(valueExpression->GetConstantValueIndex());

        unsigned idx = compilerContext.AddArrayConstantValue(constValue);
        arrayExpression->SetConstantValueIndex(idx);
    }
}

void SemanticAnalyzer::Visit(ArrayMultiValueExpression* arrayExpression)
{
    Expressions& expressions = arrayExpression->expressions;
    size_t exprsSize = expressions.size();
    assert(exprsSize > 0);

    Expression* expr = expressions[0];
    expr->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* type = expr->GetType();
    bool allExprsAreConst = expr->GetIsConstant();

    for (size_t i = 1; i < exprsSize; ++i)
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

        allExprsAreConst &= expr->GetIsConstant();
    }

    // add implicit casts if necessary
    if (type->IsInt())
    {
        for (size_t i = 0; i < exprsSize; ++i)
        {
            Expression* e = expressions[i];
            if (!e->GetType()->IsSameAs(*type))
            {
                expressions[i] = ImplicitCast(e, type);
            }
        }
    }

    const TypeInfo* arrayType = compilerContext.typeRegistry.GetArrayOfType(type);
    arrayExpression->SetType(arrayType);

    if (allExprsAreConst)
    {
        ArrayConstValue constValue;
        constValue.type = ArrayConstValue::eMultiValue;
        for (Expression* constExpr : expressions)
        {
            constValue.valueIndices.push_back(constExpr->GetConstantValueIndex());
        }

        unsigned idx = compilerContext.AddArrayConstantValue(constValue);
        arrayExpression->SetConstantValueIndex(idx);
    }
}

void SemanticAnalyzer::Visit(BlockExpression* blockExpression)
{
    // create new scope for block
    Scope scope(symbolTable);

    const SyntaxTreeNodes& statements = blockExpression->statements;
    size_t size = statements.size();

    if (size == 0)
    {
        isError = true;
        logger.LogInternalError("Block expression has no statements");
    }
    else
    {
        for (SyntaxTreeNode* statement : statements)
        {
            statement->Accept(this);
            if (isError)
            {
                break;
            }
        }

        if (!isError)
        {
            SyntaxTreeNode* lastStatement = statements[size - 1];
            if (dynamic_cast<Return*>(lastStatement) != nullptr)
            {
                blockExpression->SetType(TypeInfo::UnitType);
            }
            else
            {
                // the block expression's type is the type of its last expression
                Expression* lastExpr = dynamic_cast<Expression*>(lastStatement);
                assert(lastExpr != nullptr && "lastExpr is null");
                blockExpression->SetType(lastExpr->GetType());
            }
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

    const TypeInfo* castType = TypeExpressionToType(castExpression->typeExpression, castExpression->castToken);
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

    if (subExpression->GetIsConstant())
    {
        unsigned subExprConstIdx = subExpression->GetConstantValueIndex();

        if (exprType->IsBool())
        {
            if (castType->IsBool())
            {
                castExpression->SetConstantValueIndex(subExprConstIdx);
            }
            else if (castType->IsInt())
            {
                bool subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

                int64_t value = subValue ? 1 : 0;
                unsigned idx = compilerContext.AddIntConstantValue(value);
                castExpression->SetConstantValueIndex(idx);
            }
        }
        else if (exprType->IsInt())
        {
            int64_t subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

            if (castType->IsBool())
            {
                bool value = subValue != 0;
                unsigned idx = compilerContext.AddBoolConstantValue(value);
                castExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsInt())
            {
                int64_t value = subValue;

                unsigned exprSize = exprType->GetNumBits();
                unsigned castSize = castType->GetNumBits();
                if (castSize < exprSize)
                {
                    int64_t mask = getBitMask(castSize);
                    if (mask == 0)
                    {
                        logger.LogInternalError("Invalid int type size");
                        isError = true;
                        return;
                    }
                    value &= mask;
                }
                else if (castSize > exprSize)
                {
                    TypeInfo::ESign exprSign = exprType->GetSign();
                    if (exprSign == TypeInfo::eSigned)
                    {
                        // sign extend
                        int64_t signBit = static_cast<int64_t>(1) << (castSize - 1);
                        bool isOne = (value & signBit) != 0;
                        if (isOne)
                        {
                            unsigned size = exprSize;
                            while (size < castSize)
                            {
                                signBit <<= 1;
                                value |= signBit;
                                ++size;
                            }
                        }
                    }
                    else
                    {
                        // zero extension; nothing to do
                    }
                }
                else // sizes are equal
                {
                    // nothing to do if the sizes are equal
                }

                unsigned idx = compilerContext.AddIntConstantValue(value);
                castExpression->SetConstantValueIndex(idx);
            }
        }
    }
}

void SemanticAnalyzer::Visit(ImplicitCastExpression* castExpression)
{
    castExpression->subExpression->Accept(this);
}

void SemanticAnalyzer::Visit(FunctionCallExpression* functionCallExpression)
{
    functionCallExpression->functionExpression->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* funType = functionCallExpression->functionExpression->GetType();

    // check argument count
    const vector<Expression*>& args = functionCallExpression->arguments;
    const vector<const TypeInfo*>& paramTypes = funType->GetParamTypes();
    if (args.size() != paramTypes.size())
    {
        const char* suffix = (paramTypes.size() == 1) ? "" : "s";
        logger.LogError(*functionCallExpression->openParToken, "Function expected {} argument{} but got {}", paramTypes.size(), suffix, args.size());
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
        const TypeInfo* argType = arg->GetType();
        const TypeInfo* paramType = paramTypes[i];

        bool needsCast = false;
        if (!AreCompatibleAssignmentTypes(paramType, argType, needsCast))
        {
            logger.LogError(*functionCallExpression->openParToken, "Argument type does not match parameter type. Argument: '{}', parameter: '{}'", argType->GetShortName(), paramType->GetShortName());
            isError = true;
            return;
        }

        if (needsCast)
        {
            functionCallExpression->arguments[i] = ImplicitCast(arg, paramType);
        }
    }

    // set the expression's function type
    functionCallExpression->functionType = funType;

    // set expression's type to the function's return type
    functionCallExpression->SetType(funType->GetReturnType());
}

void SemanticAnalyzer::Visit(MemberExpression* memberExpression)
{
    Expression* expr = memberExpression->subExpression;
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
    const string& memberName = memberExpression->memberName;
    const MemberInfo* member = type->GetMember(memberName);
    if (member == nullptr)
    {
        const Token* memberToken = memberExpression->memberNameToken;
        logger.LogError(*memberToken, "Type '{}' has no member named '{}'", type->GetShortName(), memberName);
        isError = true;
        return;
    }

    memberExpression->SetType(member->GetType());
    memberExpression->SetIsStorage(expr->GetIsStorage() && member->GetIsStorage());

    if (expr->GetIsConstant())
    {
        if (type->IsStr())
        {
            // check if it's the 'Size' member
            if (member->GetIndex() == 0)
            {
                vector<char> strValue = compilerContext.GetStrConstantValue(expr->GetConstantValueIndex());
                unsigned idx = compilerContext.AddIntConstantValue(strValue.size());
                memberExpression->SetConstantValueIndex(idx);
            }
        }
        else if (type->IsArray())
        {
            // check if it's the 'Size' member
            if (member->GetIndex() == 0)
            {
                size_t arraySize = compilerContext.GetArrayConstantValueSize(expr->GetConstantValueIndex());
                unsigned idx = compilerContext.AddIntConstantValue(arraySize);
                memberExpression->SetConstantValueIndex(idx);
            }
        }
        else if (type->IsRange())
        {
            unsigned rangeIdx = expr->GetConstantValueIndex();
            RangeConstValue rangeValue = compilerContext.GetRangeConstantValue(rangeIdx);

            unsigned memberIdx = member->GetIndex();
            int64_t memberValue = 0;
            if (memberIdx == 0)
            {
                memberValue = rangeValue.start;
            }
            else
            {
                memberValue = rangeValue.end;
            }

            unsigned idx = compilerContext.AddIntConstantValue(memberValue);
            memberExpression->SetConstantValueIndex(idx);
        }
        else if (type->IsAggregate())
        {
            unsigned structIdx = expr->GetConstantValueIndex();
            StructConstValue structValue = compilerContext.GetStructConstantValue(structIdx);
            unsigned memberIdx = structValue.memberIndices[member->GetIndex()];
            memberExpression->SetConstantValueIndex(memberIdx);
        }
    }
}

void SemanticAnalyzer::Visit(BranchExpression* branchExpression)
{
    Expression* ifCondition = branchExpression->ifCondition;
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

    Expression* ifExpression = branchExpression->ifExpression;
    ifExpression->Accept(this);
    if (isError)
    {
        return;
    }

    Expression* elseExpression = branchExpression->elseExpression;
    elseExpression->Accept(this);
    if (isError)
    {
        return;
    }

    // check the "if" and "else" expression return types
    const TypeInfo* ifType = ifExpression->GetType();
    const TypeInfo* elseType = elseExpression->GetType();
    const TypeInfo* resultType = nullptr;
    if (ifType->IsInt() && elseType->IsInt() && HaveCompatibleSigns(ifType, elseType))
    {
        unsigned ifSize = ifType->GetNumBits();
        unsigned elseSize = elseType->GetNumBits();
        const NumericLiteralType* ifNumLit = dynamic_cast<const NumericLiteralType*>(ifType);
        const NumericLiteralType* elseNumLit = dynamic_cast<const NumericLiteralType*>(elseType);
        bool ifIsNumLit = ifNumLit != nullptr;
        bool elseIsNumLit = elseNumLit != nullptr;

        if (ifIsNumLit && elseIsNumLit)
        {
            resultType = GetBiggestNumLitSizeType(ifNumLit, elseNumLit);
        }
        else if (ifIsNumLit && !elseIsNumLit)
        {
            unsigned ifSize = 0;
            unsigned elseSize = 0;
            resultType = GetBiggestSizeType(ifNumLit, elseType, ifSize, elseSize);
            FixNumericLiteralExpression(branchExpression->ifExpression, resultType);
            if (ifSize > elseSize)
            {
                branchExpression->elseExpression = ImplicitCast(branchExpression->elseExpression, resultType);
            }
        }
        else if (!ifIsNumLit && elseIsNumLit)
        {
            unsigned ifSize = 0;
            unsigned elseSize = 0;
            resultType = GetBiggestSizeType(elseNumLit, ifType, elseSize, ifSize);
            FixNumericLiteralExpression(branchExpression->elseExpression, resultType);
            if (elseSize > ifSize)
            {
                branchExpression->ifExpression = ImplicitCast(branchExpression->ifExpression, resultType);
            }
        }
        else // if (!ifIsNumLit && !elseIsNumLit)
        {
            if (ifSize > elseSize)
            {
                branchExpression->elseExpression = ImplicitCast(branchExpression->elseExpression, ifType);
                resultType = ifType;
            }
            else if (ifSize < elseSize)
            {
                branchExpression->ifExpression = ImplicitCast(branchExpression->ifExpression, elseType);
                resultType = elseType;
            }
            else // both sizes are the same
            {
                resultType = ifType;
            }
        }
    }
    else if (AreCompatibleRanges(ifType, elseType, /*out*/ resultType))
    {
        const NumericLiteralType* ifInnerType = dynamic_cast<const NumericLiteralType*>(ifType->GetInnerType());
        const NumericLiteralType* elseInnerType = dynamic_cast<const NumericLiteralType*>(elseType->GetInnerType());
        bool ifIsNumLit = ifInnerType != nullptr;
        bool elseIsNumLit = elseInnerType != nullptr;

        if (ifIsNumLit && !elseIsNumLit)
        {
            FixNumericLiteralExpression(branchExpression->ifExpression, resultType);
        }
        else if (!ifIsNumLit && elseIsNumLit)
        {
            FixNumericLiteralExpression(branchExpression->elseExpression, resultType);
        }
    }
    else if (ifType->IsSameAs(*elseType))
    {
        resultType = ifType;
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

void SemanticAnalyzer::Visit(ConstantDeclaration* constantDeclaration)
{
    BinaryExpression* assignmentExpression = constantDeclaration->assignmentExpression;
    if (assignmentExpression->op != BinaryExpression::eAssign)
    {
        isError = true;
        logger.LogInternalError("Binary expression in constant declaration is not an assignment");
        return;
    }

    // process right of assignment expression before adding constant to symbol
    // table in order to detect if the constant is referenced before it is assigned
    Expression* rightExpr = assignmentExpression->right;
    rightExpr->Accept(this);
    if (isError)
    {
        return;
    }

    if (!rightExpr->GetIsConstant())
    {
        isError = true;
        logger.LogError(*assignmentExpression->opToken, "Assigned expression is not a constant value");
        return;
    }

    // set the variable type
    const TypeInfo* type = GetVariableType(constantDeclaration->typeExpression, rightExpr->GetType(), constantDeclaration->nameToken);
    if (isError)
    {
        return;
    }
    constantDeclaration->constantType = type;

    // calculate value of assignment expression
    unsigned constIdx = rightExpr->GetConstantValueIndex();

    // add the constant name to the symbol table
    const string& constName = constantDeclaration->name;
    bool ok = symbolTable.AddConstant(constName, constantDeclaration->nameToken, constantDeclaration->constantType, constIdx);
    if (!ok)
    {
        isError = true;
        LogExistingIdentifierError(constName, constantDeclaration->nameToken);
        return;
    }

    // assignment expression type check
    isConstDecl = true; // TODO: isConstDecl is a bit hacky. Maybe fix by seeing if const has been initialized yet?
    assignmentExpression->Accept(this);
    isConstDecl = false;
    if (isError)
    {
        return;
    }
}

void SemanticAnalyzer::Visit(VariableDeclaration* variableDeclaration)
{
    BinaryExpression* assignmentExpression = variableDeclaration->assignmentExpression;
    if (assignmentExpression->op != BinaryExpression::eAssign)
    {
        isError = true;
        logger.LogInternalError("Binary expression in variable declaration is not an assignment");
        return;
    }

    // process right of assignment expression before adding variable to symbol
    // table in order to detect if the variable is referenced before it is assigned
    Expression* rightExpr = assignmentExpression->right;
    rightExpr->Accept(this);
    if (isError)
    {
        return;
    }

    // set the variable type
    const TypeInfo* type = GetVariableType(variableDeclaration->typeExpression, rightExpr->GetType(), variableDeclaration->nameToken);
    if (type == nullptr)
    {
        isError = true;
        return;
    }
    variableDeclaration->variableType = type;

    // add the variable name to the symbol table
    const string& varName = variableDeclaration->name;
    bool ok = symbolTable.AddVariable(varName, variableDeclaration->nameToken, variableDeclaration->variableType);
    if (!ok)
    {
        isError = true;
        LogExistingIdentifierError(varName, variableDeclaration->nameToken);
        return;
    }

    assignmentExpression->Accept(this);
    if (isError)
    {
        return;
    }
}

bool SemanticAnalyzer::SetFunctionDeclarationTypes(FunctionDeclaration* functionDeclaration)
{
    unordered_set<string> processedParams;

    // set parameter types
    for (Parameter* param : functionDeclaration->parameters)
    {
        const string& paramName = param->name;
        if (processedParams.find(paramName) != processedParams.end())
        {
            const Token* paramToken = param->nameToken;
            logger.LogError(*paramToken, "Function '{}' has multiple parameters named '{}'", functionDeclaration->name, paramName);
            return false;
        }

        const TypeInfo* paramType = TypeExpressionToType(param->typeExpression, param->nameToken);
        if (paramType == nullptr)
        {
            return false;
        }

        param->type = paramType;
        processedParams.insert(paramName);
    }

    // set return type
    const TypeInfo* returnType = nullptr;
    Expression* returnTypeExpr = functionDeclaration->returnTypeExpression;
    if (returnTypeExpr == nullptr)
    {
        returnType = TypeInfo::UnitType;
    }
    else
    {
        returnType = TypeExpressionToType(returnTypeExpr, functionDeclaration->nameToken);
        if (returnType == nullptr)
        {
            return false;
        }
    }

    functionDeclaration->returnType = returnType;

    // the subscript operator uses exit() if a bounds check fails,
    // so we need to make sure it has the right signature
    if (functionDeclaration->name == "exit")
    {
        const vector<Parameter*>& params = functionDeclaration->parameters;
        if (params.size() != 1 || !params[0]->type->IsSameAs(*TypeInfo::Int32Type))
        {
            logger.LogError(*functionDeclaration->nameToken, "Function 'exit' must have exactly one parameter with type 'i32'");
            return false;
        }
    }
    // the subscript operator optionally uses logError() if a bounds check fails,
    // so we need to make sure it has the right signature
    else if (functionDeclaration->name == "logError")
    {
        const vector<Parameter*>& params = functionDeclaration->parameters;
        if (params.size() != 3
        || !params[0]->type->IsStr()
        || !params[1]->type->IsSameAs(*TypeInfo::UInt32Type)
        || !params[2]->type->IsStr())
        {
            logger.LogError(*functionDeclaration->nameToken, "Function 'logError' must have the following parameter types: 'str', 'u32', 'str'");
            return false;
        }
    }

    return true;
}

const TypeInfo* SemanticAnalyzer::InferType(const TypeInfo* inferType, const Token* errorToken)
{
    const TypeInfo* type = inferType;

    // check if this is an integer literal
    const TypeInfo* checkType = type;
    bool isArray = checkType->IsArray();
    while (checkType->IsArray())
    {
        checkType = checkType->GetInnerType();
    }

    bool isRange = checkType->IsRange();
    if (isRange)
    {
        checkType = checkType->GetInnerType();
    }

    const NumericLiteralType* literalType = dynamic_cast<const NumericLiteralType*>(checkType);
    if (literalType != nullptr)
    {
        const char* typeMsg = nullptr;
        if (isArray)
        {
            if (isRange)
            {
                typeMsg = "array of range literals";
            }
            else
            {
                typeMsg = "array of integer literals";
            }
        }
        else if (isRange)
        {
            typeMsg = "range literals";
        }
        else
        {
            typeMsg = "integer literals";
        }

        isError = true;
        logger.LogError(*errorToken, "Type inference is not allowed for {}", typeMsg);
        return nullptr;
    }

    return type;
}

const TypeInfo* SemanticAnalyzer::GetVariableType(Expression* typeExpression, const TypeInfo* inferType, const Token* errorToken)
{
    const TypeInfo* type = nullptr;

    // if no type name was given, infer it from the expression
    if (typeExpression == nullptr)
    {
        type = InferType(inferType, errorToken);
    }
    else // get the type from the name given
    {
        type = TypeExpressionToType(typeExpression, errorToken);
    }

    return type;
}

bool SemanticAnalyzer::CheckReturnType(const FunctionDeclaration* funcDecl, Expression* expression, const Token* errorToken, Expression*& resultExpression)
{
    resultExpression = nullptr;

    const TypeInfo* returnType = funcDecl->returnType;
    const TypeInfo* expressionType = expression->GetType();
    bool needsCast = false;
    if (!AreCompatibleAssignmentTypes(returnType, expressionType, needsCast))
    {
        logger.LogError(*errorToken, "Function '{}' has an invalid return type. Expected '{}' but got '{}'", funcDecl->name, returnType->GetShortName(), expressionType->GetShortName());
        return false;
    }

    if (needsCast)
    {
        resultExpression = ImplicitCast(expression, returnType);
    }

    return true;
}

Expression* SemanticAnalyzer::ImplicitCast(Expression* expression, const TypeInfo* type)
{
    if (expression->GetType()->IsNumericLiteral())
    {
        FixNumericLiteralExpression(expression, type);

        return expression;
    }
    else
    {
        ImplicitCastExpression* implicitCast = new ImplicitCastExpression(expression);
        implicitCast->SetType(type);
        implicitCast->SetConstantValueIndex(expression->GetConstantValueIndex());

        return implicitCast;
    }
}
