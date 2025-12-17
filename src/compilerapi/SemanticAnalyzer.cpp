#include "CompilerContext.h"
#include "ErrorLogger.h"
#include "keywords.h"
#include "SemanticAnalyzer.h"
#include "SyntaxTree.h"
#include "utils.h"
#include <cassert>
#include <cmath>
#include <sstream>

using namespace std;
using namespace SyntaxTree;

void StartEndTokenFinder::Visit(UnaryExpression* unaryExpression)
{
    UpdateStart(unaryExpression->opToken);
    unaryExpression->subExpression->Accept(this);
}

void StartEndTokenFinder::Visit(BinaryExpression* binaryExpression)
{
    binaryExpression->left->Accept(this);

    if (binaryExpression->op == BinaryExpression::eSubscript)
    {
        UpdateEnd(binaryExpression->opToken2);
    }
    else
    {
        binaryExpression->right->Accept(this);
    }
}

void StartEndTokenFinder::Visit(WhileLoop* /*whileLoop*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(ForLoop* /*forLoop*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(LoopControl* /*loopControl*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(Return* /*ret*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(FunctionTypeExpression* functionTypeExpression)
{
    UpdateStart(functionTypeExpression->funToken);

    Expression* retTypeExpr = functionTypeExpression->returnTypeExpression;
    if (retTypeExpr == nullptr)
    {
        UpdateEnd(functionTypeExpression->closeParToken);
    }
    else
    {
        retTypeExpr->Accept(this);
    }
}

void StartEndTokenFinder::Visit(ExternFunctionDeclaration* /*externFunctionDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(FunctionDefinition* /*functionDefinition*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    UpdateStart(structDefinitionExpression->structToken);
    UpdateEnd(structDefinitionExpression->closeBraceToken);
}

void StartEndTokenFinder::Visit(StructInitializationExpression* structInitializationExpression)
{
    structInitializationExpression->structTypeExpression->Accept(this);
    UpdateEnd(structInitializationExpression->closeBraceToken);
}

void StartEndTokenFinder::Visit(ModuleDefinition* /*moduleDefinition*/)
{
    assert(false && "StartEndTokenFinder does not support ModuleDefinition");
}

void StartEndTokenFinder::Visit(Modules* /*modules*/)
{
    assert(false && "StartEndTokenFinder does not support Modules");
}

void StartEndTokenFinder::Visit(UnitTypeLiteralExpression* /*unitTypeLiteralExpression*/)
{
}

void StartEndTokenFinder::Visit(NumericExpression* numericExpression)
{
    const Token* token = numericExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    const Token* token = floatLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    const Token* token = boolLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(StringLiteralExpression* stringLiteralExpression)
{
    const Token* token = stringLiteralExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(IdentifierExpression* identifierExpression)
{
    const Token* token = identifierExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(BuiltInIdentifierExpression* builtInIdentifierExpression)
{
    const Token* token = builtInIdentifierExpression->token;
    UpdateStart(token);
    UpdateEnd(token);
}

void StartEndTokenFinder::Visit(ArraySizeValueExpression* arrayExpression)
{
    UpdateStart(arrayExpression->startToken);
    UpdateEnd(arrayExpression->endToken);
}

void StartEndTokenFinder::Visit(ArrayMultiValueExpression* arrayExpression)
{
    UpdateStart(arrayExpression->startToken);
    UpdateEnd(arrayExpression->endToken);
}

void StartEndTokenFinder::Visit(BlockExpression* blockExpression)
{
    UpdateStart(blockExpression->startToken);
    UpdateEnd(blockExpression->endToken);
}

void StartEndTokenFinder::Visit(UncheckedBlock* /*uncheckedBlock*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(ImplicitCastExpression* castExpression)
{
    castExpression->Accept(this);
}

void StartEndTokenFinder::Visit(FunctionCallExpression* functionCallExpression)
{
    functionCallExpression->functionExpression->Accept(this);
    UpdateEnd(functionCallExpression->closeParToken);
}

void StartEndTokenFinder::Visit(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    UpdateStart(builtInFunctionCallExpression->nameToken);
    UpdateEnd(builtInFunctionCallExpression->closeParToken);
}

void StartEndTokenFinder::Visit(MemberExpression* memberExpression)
{
    memberExpression->subExpression->Accept(this);
    UpdateEnd(memberExpression->memberNameToken);
}

void StartEndTokenFinder::Visit(BranchExpression* branchExpression)
{
    UpdateStart(branchExpression->ifToken);
    branchExpression->elseExpression->Accept(this);
}

void StartEndTokenFinder::Visit(ConstantDeclaration* /*constantDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::Visit(VariableDeclaration* /*variableDeclaration*/)
{
    assert(false && "StartEndTokenFinder member function is not implemented");
}

void StartEndTokenFinder::UpdateStart(const Token* newStart)
{
    if (start == nullptr || newStart->line < start->line || (newStart->line == start->line && newStart->column < start->column))
    {
        start = newStart;
    }
}

void StartEndTokenFinder::UpdateEnd(const Token* newEnd)
{
    if (end == nullptr || newEnd->line > end->line || (newEnd->line == end->line && newEnd->column > end->column))
    {
        end = newEnd;
    }
}

SemanticAnalyzer::SemanticAnalyzer(CompilerContext& compilerContext) :
    logger(compilerContext.logger),
    loopLevel(0),
    uncheckedLevel(0),
    isError(false),
    isConstDecl(false),
    compilerContext(compilerContext),
    symbolTable(compilerContext),
    currentFunction(nullptr)
{
    // create a default entry for struct definition expressions
    incompleteStructExpressions.push(new vector<StructDefinitionExpression*>());
}

SemanticAnalyzer::~SemanticAnalyzer()
{
    // in an error case, there might be invalid pointers on the stack, so we need
    // to pop them off
    while (incompleteStructExpressions.size() > 1)
    {
        incompleteStructExpressions.pop();
    }

    delete incompleteStructExpressions.top();
}

bool SemanticAnalyzer::Process(SyntaxTreeNode* syntaxTree)
{
    syntaxTree->Accept(this);
    return !isError;
}

void SemanticAnalyzer::Visit(UnaryExpression* unaryExpression)
{
    UnaryExpression::EOperator op = unaryExpression->op;

    Expression* subExpr = unaryExpression->subExpression;
    subExpr->Accept(this);
    if (isError)
    {
        return;
    }

    const TypeInfo* subExprType = subExpr->GetType();
    bool isInt = subExprType->IsInt();
    bool isFloat = subExprType->IsFloat();

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

                    resultType = compilerContext.typeRegistry.CreateSignedNumericLiteralType(subExprLiteralType->GetSignedNumBits());
                    ok = true;
                }
            }
            else if (isFloat)
            {
                ok = true;
                resultType = subExpr->GetType();
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
        ROString opString = UnaryExpression::GetOperatorString(op);
        logger.LogError(*unaryExpression->opToken, "Unary operator '{}' does not support type '{}'", opString, subExprType->GetName());
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
        else if (subExprType->IsFloat())
        {
            if (op == UnaryExpression::eNegative)
            {
                double subValue = compilerContext.GetFloatConstantValue(subExpr->GetConstantValueIndex());
                unsigned idx = compilerContext.AddFloatConstantValue(-subValue);
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
    if ( (binaryExpression->left->GetType()->IsInt() && binaryExpression->right->GetType()->IsInt())
      || (binaryExpression->left->GetType()->IsFloat() && binaryExpression->right->GetType()->IsFloat()) )
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
        ROString opString = BinaryExpression::GetOperatorString(op);
        logger.LogError(
            *opToken,
            "Binary operator '{}' does not support types '{}' and '{}'",
            opString,
            binaryExpression->left->GetType()->GetName(),
            binaryExpression->right->GetType()->GetName());

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
        else if (leftType->IsFloat() && rightType->IsFloat())
        {
            double leftValue = compilerContext.GetFloatConstantValue(binaryExpression->left->GetConstantValueIndex());
            double rightValue = compilerContext.GetFloatConstantValue(binaryExpression->right->GetConstantValueIndex());

            bool isConst = true;
            bool boolValue = false;
            double doubleValue = 0.0;
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
                    doubleValue = leftValue + rightValue;
                    break;
                case BinaryExpression::eSubtract:
                    doubleValue = leftValue - rightValue;
                    break;
                case BinaryExpression::eMultiply:
                    doubleValue = leftValue * rightValue;
                    break;
                case BinaryExpression::eDivide:
                    doubleValue = leftValue / rightValue;
                    break;
                case BinaryExpression::eRemainder:
                    doubleValue = fmod(leftValue, rightValue);
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
                else if (resultType->IsFloat())
                {
                    idx = compilerContext.AddFloatConstantValue(doubleValue);
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
                    StartEndTokenFinder finder;
                    binaryExpression->right->Accept(&finder);

                    logger.LogError(
                        *finder.start, *finder.end,
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
                    StartEndTokenFinder finder;
                    binaryExpression->right->Accept(&finder);

                    logger.LogError(
                        *finder.start, *finder.end,
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
    if (!assignType->IsOrContainsLiteral() && !exprType->IsOrContainsLiteral() && assignType->IsSameAs(*exprType))
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

    if (assignType->IsFloat() && exprType->IsFloat() && assignType->GetNumBits() >= exprType->GetNumBits())
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

    if (assignInnerType->IsInt() && exprInnerType->IsOrContainsNumericLiteral()
        && HaveCompatibleSigns(assignInnerType, exprInnerType)
        && HaveCompatibleAssignmentSizes(assignInnerType, exprInnerType))
    {
        needsCast = true;
        return true;
    }

    if (assignInnerType->IsFloat() && exprInnerType->IsFloat() && exprInnerType->IsLiteral()
        && assignInnerType->GetNumBits() >= exprInnerType->GetNumBits())
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
        resultType = compilerContext.typeRegistry.CreateSignedNumericLiteralType(signedNumBits);
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
    else if (type1->IsFloat() && type2->IsFloat() && (type1->IsLiteral() || type2->IsLiteral()))
    {
        if (type1->IsLiteral() && type2->IsLiteral())
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
        else
        {
            unsigned numBits = max(type1->GetNumBits(), type2->GetNumBits());
            if (numBits == 32)
            {
                resultType = TypeInfo::Float32Type;
            }
            else
            {
                resultType = TypeInfo::Float64Type;
            }
        }
    }
    else // neither types are NumericLiteralType or float literals
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
    else if (FloatLiteralExpression* floatExpr = dynamic_cast<FloatLiteralExpression*>(expr); floatExpr != nullptr)
    {
        floatExpr->SetType(resultType);
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
    else if (BuiltInIdentifierExpression* builtInIdExpr = dynamic_cast<BuiltInIdentifierExpression*>(expr); builtInIdExpr != nullptr)
    {
        builtInIdExpr->SetType(resultType);
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
    else if (leftType->IsFloat() && rightType->IsFloat())
    {
        switch (op)
        {
            case BinaryExpression::eEqual:
            case BinaryExpression::eNotEqual:
            case BinaryExpression::eLessThan:
            case BinaryExpression::eLessThanOrEqual:
            case BinaryExpression::eGreaterThan:
            case BinaryExpression::eGreaterThanOrEqual:
            case BinaryExpression::eAdd:
            case BinaryExpression::eSubtract:
            case BinaryExpression::eMultiply:
            case BinaryExpression::eDivide:
            case BinaryExpression::eRemainder:
            case BinaryExpression::eAssign:
            case BinaryExpression::eAddAssign:
            case BinaryExpression::eSubtractAssign:
            case BinaryExpression::eMultiplyAssign:
            case BinaryExpression::eDivideAssign:
            case BinaryExpression::eRemainderAssign:
                ok = leftType->GetNumBits() == rightType->GetNumBits();
                break;
            default:
                ok = false;
                break;
        }

        if (ok)
        {
            if (leftType->IsLiteral() && !rightType->IsLiteral())
            {
                const TypeInfo* resultType = GetBiggestSizeType(leftType, rightType);
                FixNumericLiteralExpression(left, resultType);
                if (right->GetType()->GetNumBits() != resultType->GetNumBits())
                {
                    binExpr->right = ImplicitCast(right, resultType);
                }
            }
            else if (!leftType->IsLiteral() && rightType->IsLiteral())
            {
                const TypeInfo* resultType = GetBiggestSizeType(leftType, rightType);
                FixNumericLiteralExpression(right, resultType);
                if (left->GetType()->GetNumBits() != resultType->GetNumBits())
                {
                    binExpr->left = ImplicitCast(left, resultType);
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
            else if (leftType->IsFloat() && rightType->IsFloat())
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
        StartEndTokenFinder finder;
        condition->Accept(&finder);

        isError = true;
        logger.LogError(
            *finder.start, *finder.end,
            "While loop condition must be a boolean expression"
        );
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
        StartEndTokenFinder finder;
        iterExpression->Accept(&finder);

        isError = true;
        logger.LogError(*finder.start, *finder.end, "For loop expression is not iterable");
        return;
    }

    // set variable type
    const TypeInfo* varType = GetVariableType(forLoop->varTypeExpression, inferType, forLoop->variableNameToken);
    if (isError)
    {
        return;
    }

    // make sure the type is not 'type', '[]type', '&type', etc.
    const TypeInfo* innerMostType = varType;
    while (innerMostType->GetInnerType() != nullptr)
    {
        innerMostType = innerMostType->GetInnerType();
    }
    if (innerMostType->IsType())
    {
        isError = true;
        logger.LogError(*forLoop->variableNameToken, "Variable cannot be of type '{}'", varType->GetName());
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

            inferType = compilerContext.typeRegistry.CreateNumericLiteralType(varType->GetSign(), arrayInnerLiteralType->GetSignedNumBits(), arrayInnerLiteralType->GetUnsignedNumBits());
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
    ROString varName = forLoop->variableName;
    bool ok = symbolTable.AddVariable(varName, forLoop->variableNameToken, forLoop->variableType);
    if (!ok)
    {
        isError = true;
        LogExistingIdentifierError(varName, forLoop->variableNameToken);
        return;
    }

    // add the index variable name to the symbol table
    ROString indexVarName = forLoop->indexName;
    if (indexVarName.GetSize() > 0)
    {
        ok = symbolTable.AddVariable(indexVarName, forLoop->indexNameToken, forLoop->indexType);
        if (!ok)
        {
            isError = true;
            LogExistingIdentifierError(indexVarName, forLoop->indexNameToken);
            return;
        }
    }

    // check variable type and iterable type
    bool needsCast = false;
    if (!AreCompatibleAssignmentTypes(varType, inferType, needsCast))
    {
        isError = true;
        logger.LogError(
            *forLoop->variableNameToken,
            "Cannot assign value of type '{}' to variable '{}' of type '{}'",
            inferType->GetName(),
            varName,
            varType->GetName()
        );
        return;
    }

    if (needsCast)
    {
        const TypeInfo* newType = nullptr;
        if (iterExprType->IsRange())
        {
            newType = compilerContext.typeRegistry.GetRangeType(varType, iterExprType->IsHalfOpen());
        }
        else if (iterExprType->IsArray())
        {
            newType = compilerContext.typeRegistry.GetArrayOfType(varType);
        }

        assert(newType != nullptr && "Unable to cast for loop iter expression");

        const TypeInfo* iterExprType = iterExpression->GetType();
        if (iterExprType->IsOrContainsNumericLiteral() || iterExprType->IsOrContainsLiteral())
        {
            FixNumericLiteralExpression(iterExpression, newType);
        }
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

        const TypeInfo* paramType = TypeExpressionToType(paramTypeExpr);
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

        returnType = TypeExpressionToType(functionTypeExpression->returnTypeExpression);
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
        ROString paramName = param->name;
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

void SemanticAnalyzer::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    TypeInfo* structType = nullptr;
    unsigned constIdx = -1;

    bool needsStructImpl = needsStructImplStack.top();

    // if the constant index is set, this struct already has a declaration, so get it
    if (structDefinitionExpression->GetIsConstant())
    {
        constIdx = structDefinitionExpression->GetConstantValueIndex();
        auto iter = incompleteStructTypes.find(constIdx);
        assert(iter != incompleteStructTypes.end() && "could not find incomplete struct");
        structType = iter->second;
    }
    // otherwise, this is a new struct
    else
    {
        structType = compilerContext.typeRegistry.GetStructType("", structDefinitionExpression->structToken);

        // set the struct's name if it does not already have one
        if (structDefinitionExpression->name.IsEmpty())
        {
            structDefinitionExpression->name = compilerContext.stringBuilder
                .Append("<struct", to_string(structType->GetId()), ">")
                .CreateString();
        }

        // set the type's name
        structType->name = structDefinitionExpression->name;

        structDefinitionExpression->SetType(TypeInfo::TypeType);

        constIdx = compilerContext.AddTypeConstantValue(structType);
        structDefinitionExpression->SetConstantValueIndex(constIdx);

        if (!needsStructImpl)
        {
            incompleteStructTypes.insert({constIdx, structType});
            incompleteStructExpressions.top()->push_back(structDefinitionExpression);
        }
    }

    // only process the members if this is not a declaration
    if (needsStructImpl)
    {
        vector<const Token*> structTokenStack;
        for (MemberDefinition* member : structDefinitionExpression->members)
        {
            structTokenStack.clear();

            const TypeInfo* memberType = TypeExpressionToType(member->typeExpression);
            if (memberType == nullptr)
            {
                isError = true;
                return;
            }

            // make sure the type is not 'type', '[]type', '&type', etc.
            const TypeInfo* innerMostType = memberType;
            while (innerMostType->GetInnerType() != nullptr)
            {
                innerMostType = innerMostType->GetInnerType();
            }
            if (innerMostType->IsType())
            {
                isError = true;
                logger.LogError(*member->nameToken, "Member cannot be of type '{}'", memberType->GetName());
                return;
            }

            // check if member type creates a recursive dependency
            if (memberType->IsRecursiveStructDependency(structType->GetName(), structTokenStack))
            {
                isError = true;
                logger.LogError(*member->nameToken, "Member '{}' creates a recursive dependency", member->name);

                for (auto iter = structTokenStack.cbegin(); iter != structTokenStack.cend(); ++iter)
                {
                    const Token* token = *iter;
                    logger.LogNote(*token, "Dependency on '{}'", token->value);
                }

                return;
            }

            // if there is a default member expression, process it
            if (member->defaultValueExpression != nullptr)
            {
                member->defaultValueExpression->Accept(this);
                if (isError)
                {
                    return;
                }

                bool needsCast = false;
                if (!AreCompatibleAssignmentTypes(memberType, member->defaultValueExpression->GetType(), needsCast))
                {
                    StartEndTokenFinder finder;
                    member->defaultValueExpression->Accept(&finder);

                    logger.LogError(
                        *finder.start, *finder.end,
                        "Cannot assign expression of type '{}' to member '{}' of type '{}'",
                        member->defaultValueExpression->GetType()->GetName(),
                        member->name,
                        memberType->GetName()
                    );

                    isError = true;
                    return;
                }

                if (needsCast)
                {
                    member->defaultValueExpression = ImplicitCast(member->defaultValueExpression, memberType);
                }

                // check if expression is constant
                if (!member->defaultValueExpression->GetIsConstant())
                {
                    StartEndTokenFinder finder;
                    member->defaultValueExpression->Accept(&finder);

                    logger.LogError(
                        *finder.start, *finder.end,
                        "Default member value is not a constant expression"
                    );

                    isError = true;
                    return;
                }
            }

            ROString memberName = member->name;
            bool added = structType->AddMember(memberName, memberType, true, member->nameToken);
            if (!added)
            {
                isError = true;
                logger.LogError(*member->nameToken, "Duplicate member '{}' in struct '{}'", memberName, structType->GetName());
                return;
            }
        }

        incompleteStructTypes.erase(constIdx);
    }
}

void SemanticAnalyzer::Visit(StructInitializationExpression* structInitializationExpression)
{
    Expression* structTypeExpr = structInitializationExpression->structTypeExpression;

    structTypeExpr->Accept(this);
    if (isError)
    {
        return;
    }

    // check if type expression is a constant
    if (!structTypeExpr->GetIsConstant())
    {
        StartEndTokenFinder finder;
        structTypeExpr->Accept(&finder);

        isError = true;
        logger.LogError(*finder.start, *finder.end, "Struct type must be a constant expression");
        return;
    }

    // check if type expression is a type
    if (!structTypeExpr->GetType()->IsType())
    {
        StartEndTokenFinder finder;
        structTypeExpr->Accept(&finder);

        isError = true;
        logger.LogError(*finder.start, *finder.end, "Expression is not a type");
        return;
    }

    // check if type is a struct
    const TypeInfo* type = compilerContext.GetTypeConstantValue(structTypeExpr->GetConstantValueIndex());
    if (!type->IsStruct())
    {
        StartEndTokenFinder finder;
        structTypeExpr->Accept(&finder);

        isError = true;
        logger.LogError(*finder.start, *finder.end, "Expression value is not a struct type");
        return;
    }

    structInitializationExpression->SetType(type);

    unordered_set<ROString> membersToInit;
    for (const MemberInfo* member : type->GetMembers())
    {
        membersToInit.insert(member->GetName());
    }

    bool allMembersAreConst = true;
    for (MemberInitialization* member : structInitializationExpression->memberInitializations)
    {
        ROString memberName = member->name;

        // get member info
        const MemberInfo* memberInfo = type->GetMember(memberName);
        if (memberInfo == nullptr)
        {
            isError = true;
            ROString structName = type->GetName();
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
            StartEndTokenFinder finder;
            expr->Accept(&finder);

            isError = true;
            logger.LogError(
                *finder.start, *finder.end,
                "Cannot assign expression of type '{}' to member '{}' of type '{}'",
                exprType->GetName(), memberName, memberType->GetName());
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
        stringstream errorMsg;
        if (membersNotInit == 1)
        {
            errorMsg << "Struct member '" << *membersToInit.cbegin() << "' was not initialized";
        }
        else
        {
            auto iter = membersToInit.cbegin();
            errorMsg << "The following struct members were not initialized: " << *iter;
            ++iter;
            for (; iter != membersToInit.cend(); ++iter)
            {
                errorMsg << ", ";
                errorMsg << *iter;
            }
        }

        isError = true;
        const Token* startToken = structInitializationExpression->openBraceToken;
        const Token* closeToken = structInitializationExpression->closeBraceToken;
        logger.LogError(*startToken, *closeToken, errorMsg.str().c_str());
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

void SemanticAnalyzer::LogExistingIdentifierError(ROString name, const Token* token, const Token* existingToken)
{
    logger.LogError(*token, "Identifier '{}' has already been declared", name);

    // attempt to look up existing token if it is not specified
    if (existingToken == nullptr)
    {
        const SymbolTable::IdentifierData* data = symbolTable.GetIdentifierData(name);
        if (data != nullptr)
        {
            existingToken = data->token;
        }
    }

    if (existingToken != nullptr)
    {
        logger.LogNote(*existingToken, "Identifier is declared here");
    }
}

void SemanticAnalyzer::ProcessConstantDeclarations(vector<ConstantDeclarations*>& constantDeclarations)
{
    // build constant name map
    for (ConstantDeclarations* inner : constantDeclarations)
    {
        for (ConstantDeclaration* constDecl : *inner)
        {
            ROString constName = constDecl->name;
            auto iter = unresolvedConsts.insert({constName, constDecl});
            if (!iter.second)
            {
                const Token* existingToken = iter.first->second->nameToken;
                isError = true;
                LogExistingIdentifierError(constName, constDecl->nameToken, existingToken);
                return;
            }
        }
    }

    // process constants, but don't process struct members yet
    needsStructImplStack.push(false);
    for (ConstantDeclarations* inner : constantDeclarations)
    {
        for (ConstantDeclaration* constDecl : *inner)
        {
            // process the constant declaration if it has not already been resolved
            if (unresolvedConsts.find(constDecl->name) != unresolvedConsts.end())
            {
                constDecl->Accept(this);
                if (isError)
                {
                    return;
                }
            }
        }
    }
}

const TypeInfo* SemanticAnalyzer::TypeExpressionToType(Expression* typeExpression)
{
    typeExpression->Accept(this);
    if (isError)
    {
        return nullptr;
    }

    if (!typeExpression->GetType()->IsType())
    {
        StartEndTokenFinder finder;
        typeExpression->Accept(&finder);

        logger.LogError(*finder.start, *finder.end, "Expected expression to be of type '{}'", TYPE_KEYWORD);
        return nullptr;
    }
    if (!typeExpression->GetIsConstant())
    {
        StartEndTokenFinder finder;
        typeExpression->Accept(&finder);

        logger.LogError(*finder.start, *finder.end, "Type must be a constant expression");
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
    vector<ConstantDeclarations*> allConstDecls;
    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        allConstDecls.push_back(&moduleDefinition->constantDeclarations);
    }

    // resolve constant declarations
    ProcessConstantDeclarations(allConstDecls);
    if (isError)
    {
        return;
    }

    // build a look-up table for all functions

    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        const vector<ExternFunctionDeclaration*>& externFuncDecls = moduleDefinition->externFunctionDeclarations;
        const vector<FunctionDefinition*>& funcDefs = moduleDefinition->functionDefinitions;

        for (ExternFunctionDeclaration* externFunc : externFuncDecls)
        {
            FunctionDeclaration* decl = externFunc->declaration;

            bool ok = SetFunctionDeclarationTypes(decl);
            if (!ok)
            {
                isError = true;
                return;
            }

            const TypeInfo* funType = compilerContext.typeRegistry.GetFunctionType(decl);
            unsigned idx = compilerContext.AddFunctionConstantValue(decl);
            externFunc->SetConstantValueIndex(idx);
            ok = symbolTable.AddConstant(decl->name, decl->nameToken, funType, idx);
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

            bool ok = SetFunctionDeclarationTypes(decl);
            if (!ok)
            {
                isError = true;
                return;
            }

            const TypeInfo* funType = compilerContext.typeRegistry.GetFunctionType(decl);
            unsigned idx = compilerContext.AddFunctionConstantValue(decl);
            funcDef->SetConstantValueIndex(idx);
            ok = symbolTable.AddConstant(decl->name, decl->nameToken, funType, idx);
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

    modules->orderedGlobalConstants.swap(orderedGlobalConsts);
    orderedGlobalConsts.clear();
}

void SemanticAnalyzer::Visit(UnitTypeLiteralExpression* unitTypeLiteralExpression)
{
    unitTypeLiteralExpression->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::Visit(NumericExpression* numericExpression)
{
    unsigned minSignedNumBits = getMinSignedSize(numericExpression->value);
    unsigned minUnsignedNumBits = getMinUnsignedSize(static_cast<uint64_t>(numericExpression->value));
    if (minSignedNumBits == 0 || minUnsignedNumBits == 0)
    {
        isError = true;
        logger.LogInternalError("Could not get type for numeric literal");
        return;
    }

    const NumericLiteralType* type = compilerContext.typeRegistry.CreateContextDependentNumericLiteralType(minSignedNumBits, minUnsignedNumBits);
    numericExpression->SetType(type);

    int64_t value = numericExpression->value;
    unsigned idx = compilerContext.AddIntConstantValue(value);
    numericExpression->SetConstantValueIndex(idx);
}

void SemanticAnalyzer::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    double value = floatLiteralExpression->value;
    const int64_t* bits = reinterpret_cast<const int64_t*>(&value);
    int64_t exp = ((*bits & 0x7ff00000'00000000) >> 52) - 1023;
    bool isZero = (*bits & 0x7fffffff'ffffffff) == 0;

    const TypeInfo* type = nullptr;
    if ( isZero || (exp >= -126 && exp <= 127) )
    {
        type = TypeInfo::Float32LiteralType;
    }
    else
    {
        type = TypeInfo::Float64LiteralType;
    }

    floatLiteralExpression->SetType(type);

    unsigned idx = compilerContext.AddFloatConstantValue(value);
    floatLiteralExpression->SetConstantValueIndex(idx);
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
    ROString name = identifierExpression->name;
    const SymbolTable::IdentifierData* data = symbolTable.GetIdentifierData(name);

    if (data == nullptr)
    {
        // check if this is a constant that has not been resolved yet
        auto iter = unresolvedConsts.find(name);

        // if it is not in the unresolved list, it is an unknown identifier
        if (iter == unresolvedConsts.end())
        {
            logger.LogError(*identifierExpression->token, "Identifier '{}' is not declared in the current scope", name);
            isError = true;
            return;
        }

        // this constant is in the unresolved list, so resolve it now
        constIdentifierStack.push_back(identifierExpression->token);
        ConstantDeclaration* constDecl = iter->second;
        constDecl->Accept(this);
        constIdentifierStack.pop_back();
        if (isError)
        {
            return;
        }

        data = symbolTable.GetIdentifierData(name);
        assert(data != nullptr && "Could not find identifier data");
    }

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

void SemanticAnalyzer::Visit(BuiltInIdentifierExpression* builtInIdentifierExpression)
{
    const Token* token = builtInIdentifierExpression->token;
    ROString name = token->value;

    const TypeInfo* type = nullptr;
    unsigned constIdx = static_cast<unsigned>(-1);
    if (name == "@e")
    {
        type = TypeInfo::Float32LiteralType;
        constIdx = compilerContext.AddFloatConstantValue(2.71828182845904523536);
    }
    else if (name == "@pi")
    {
        type = TypeInfo::Float32LiteralType;
        constIdx = compilerContext.AddFloatConstantValue(3.14159265358979323846);
    }
    else
    {
        logger.LogError(*token, "'{}' is not a valid built-in identifier", name);
        isError = true;
        return;
    }

    builtInIdentifierExpression->SetType(type);
    builtInIdentifierExpression->SetConstantValueIndex(constIdx);
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
        StartEndTokenFinder finder;
        sizeExpression->Accept(&finder);

        isError = true;
        logger.LogError(
            *finder.start, *finder.end,
            "Invalid array size. Array sizes must be unsigned constant expressions"
        );
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
            else if (exprType->IsFloat() && type->IsFloat())
            {
                if (exprType->GetNumBits() > type->GetNumBits())
                {
                    type = exprType;
                }
            }
            else
            {
                StartEndTokenFinder finder;
                expr->Accept(&finder);

                isError = true;
                logger.LogError(
                    *finder.start, *finder.end,
                    "Array item at index {} with type '{}' does not match previous item type '{}'",
                    i,
                    exprType->GetName(),
                    type->GetName()
                );
                return;
            }
        }

        allExprsAreConst &= expr->GetIsConstant();
    }

    // add implicit casts if necessary
    if (type->IsNumeric())
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

    vector<ConstantDeclarations*> constDecls;
    constDecls.push_back(&blockExpression->constantDeclarations);
    ProcessConstantDeclarations(constDecls);

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

void SemanticAnalyzer::Visit(UncheckedBlock* uncheckedBlock)
{
    ++uncheckedLevel;
    uncheckedBlock->block->Accept(this);
    --uncheckedLevel;
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
            StartEndTokenFinder finder;
            arg->Accept(&finder);

            logger.LogError(
                *finder.start, *finder.end,
                "Argument type does not match parameter type. Argument: '{}', parameter: '{}'",
                argType->GetName(),
                paramType->GetName()
            );
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

void SemanticAnalyzer::Visit(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    if (name == "@assert")
    {
        BuiltInAssert(builtInFunctionCallExpression);
    }
    else if (name == "@bitCast")
    {
        BuiltInBitCast(builtInFunctionCallExpression);
    }
    else if (name == "@cast")
    {
        BuiltInCast(builtInFunctionCallExpression);
    }
    else if (name == "@intToPtr")
    {
        BuiltInIntToPtr(builtInFunctionCallExpression);
    }
    else if (name == "@ptrToInt")
    {
        BuiltInPtrToInt(builtInFunctionCallExpression);
    }
    else
    {
        logger.LogError(*nameToken, "'{}' is not a valid built-in function", name);
        isError = true;
    }
}

void SemanticAnalyzer::BuiltInAssert(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    const Expressions& args = builtInFunctionCallExpression->arguments;
    if (args.size() != 2)
    {
        logger.LogError(*nameToken, "{} expected 2 arguments but got {}", name, args.size());
        isError = true;
        return;
    }

    const TypeInfo* paramTypes[2] = {
        TypeInfo::BoolType,
        compilerContext.typeRegistry.GetStringType(),
    };

    for (size_t i = 0; i < 2; ++i)
    {
        Expression* arg = args[i];
        arg->Accept(this);
        if (isError)
        {
            return;
        }

        const TypeInfo* argType = arg->GetType();
        const TypeInfo* paramType = paramTypes[i];

        bool needsCast = false;
        if (!AreCompatibleAssignmentTypes(paramType, argType, needsCast))
        {
            StartEndTokenFinder finder;
            arg->Accept(&finder);

            logger.LogError(
                *finder.start, *finder.end,
                "Argument type does not match parameter type. Argument: '{}', parameter: '{}'",
                argType->GetName(),
                paramType->GetName()
            );
            isError = true;
            return;
        }

        if (needsCast)
        {
            builtInFunctionCallExpression->arguments[i] = ImplicitCast(arg, paramType);
        }
    }

    builtInFunctionCallExpression->SetType(TypeInfo::UnitType);
}

void SemanticAnalyzer::BuiltInBitCast(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    const Expressions& args = builtInFunctionCallExpression->arguments;
    if (args.size() != 2)
    {
        logger.LogError(*nameToken, "{} expected 2 arguments but got {}", name, args.size());
        isError = true;
        return;
    }

    Expression* typeExpression = args[0];
    const TypeInfo* castType = TypeExpressionToType(typeExpression);
    if (castType == nullptr)
    {
        isError = true;
        return;
    }

    Expression* subExpression = args[1];
    subExpression->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* exprType = subExpression->GetType();

    // check if the cast is valid
    bool canCast = false;
    if (exprType->IsInt())
    {
        canCast = castType->IsInt() || castType->IsFloat();
    }
    else if (exprType->IsFloat())
    {
        canCast = castType->IsInt() || castType->IsFloat();
    }

    if (!canCast)
    {
        StartEndTokenFinder finder;
        subExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Cannot bit-cast expression of type '{}' to type '{}'",
            exprType->GetName(), castType->GetName()
        );
        isError = true;
        return;
    }

    // check if we need an implicit cast
    if (exprType->IsLiteral() && exprType->GetNumBits() < castType->GetNumBits())
    {
        if (exprType->IsInt())
        {
            const NumericLiteralType* newExprType = nullptr;
            if (exprType->GetSign() == TypeInfo::eSigned)
            {
                newExprType = compilerContext.typeRegistry.CreateSignedNumericLiteralType(castType->GetNumBits());
            }
            else
            {
                newExprType = compilerContext.typeRegistry.CreateUnsignedNumericLiteralType(castType->GetNumBits());
            }

            builtInFunctionCallExpression->arguments[1] = ImplicitCast(builtInFunctionCallExpression->arguments[1], newExprType);
            subExpression = builtInFunctionCallExpression->arguments[1];
            exprType = subExpression->GetType();
        }
        else if (exprType->IsFloat())
        {
            if (exprType->GetNumBits() == 32 && castType->GetNumBits() == 64)
            {
                builtInFunctionCallExpression->arguments[1] = ImplicitCast(builtInFunctionCallExpression->arguments[1], TypeInfo::Float64LiteralType);
                subExpression = builtInFunctionCallExpression->arguments[1];
                exprType = subExpression->GetType();
            }
        }
    }

    // make sure types are of the same size
    if (exprType->GetNumBits() != castType->GetNumBits())
    {
        StartEndTokenFinder finder;
        subExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Cannot bit-cast expression of type '{}' to type '{}' because sizes are not equal",
            exprType->GetName(), castType->GetName()
        );
        isError = true;
        return;
    }

    builtInFunctionCallExpression->SetType(castType);

    if (subExpression->GetIsConstant())
    {
        unsigned subExprConstIdx = subExpression->GetConstantValueIndex();

        if (exprType->IsInt())
        {
            int64_t subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

            if (castType->IsInt())
            {
                unsigned idx = compilerContext.AddIntConstantValue(subValue);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsFloat())
            {
                unsigned castSize = castType->GetNumBits();
                double value = 0.0;
                if (castSize == 32)
                {
                    int32_t subValue32 = static_cast<int32_t>(subValue);
                    float* floatValuePtr = reinterpret_cast<float*>(&subValue32);
                    float floatValue = *floatValuePtr;
                    value = static_cast<double>(floatValue);
                }
                else
                {
                    double* doubleValuePtr = reinterpret_cast<double*>(&subValue);
                    value = *doubleValuePtr;
                }

                unsigned idx = compilerContext.AddFloatConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
        }
        else if (exprType->IsFloat())
        {
            double subValue = compilerContext.GetFloatConstantValue(subExprConstIdx);

            if (castType->IsInt())
            {
                unsigned castSize = castType->GetNumBits();
                int64_t value = 0;
                if (castSize == 32)
                {
                    float floatValue = static_cast<float>(subValue);
                    uint32_t* intValuePtr = reinterpret_cast<uint32_t*>(&floatValue);
                    uint64_t intValue = static_cast<uint64_t>(*intValuePtr);
                    value = static_cast<int64_t>(intValue);
                }
                else if (castSize == 64)
                {
                    int64_t* intValuePtr = reinterpret_cast<int64_t*>(&subValue);
                    value = *intValuePtr;
                }

                unsigned idx = compilerContext.AddIntConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsFloat())
            {
                unsigned idx = compilerContext.AddFloatConstantValue(subValue);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
        }
    }
}

void SemanticAnalyzer::BuiltInCast(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    const Expressions& args = builtInFunctionCallExpression->arguments;
    if (args.size() != 2)
    {
        logger.LogError(*nameToken, "{} expected 2 arguments but got {}", name, args.size());
        isError = true;
        return;
    }

    Expression* typeExpression = args[0];
    const TypeInfo* castType = TypeExpressionToType(typeExpression);
    if (castType == nullptr)
    {
        isError = true;
        return;
    }

    Expression* subExpression = args[1];
    subExpression->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* exprType = subExpression->GetType();

    // check if the cast is valid
    bool canCast = false;
    if (exprType->IsBool())
    {
        canCast = castType->IsBool() || castType->IsInt() || castType->IsFloat();
    }
    else if (exprType->IsInt())
    {
        canCast = castType->IsBool() || castType->IsInt() || castType->IsFloat();
    }
    else if (exprType->IsFloat())
    {
        canCast = castType->IsBool() || castType->IsInt() || castType->IsFloat();
    }

    if (!canCast)
    {
        StartEndTokenFinder finder;
        subExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Cannot cast expression of type '{}' to type '{}'",
            exprType->GetName(), castType->GetName()
        );
        isError = true;
        return;
    }

    builtInFunctionCallExpression->SetType(castType);

    if (subExpression->GetIsConstant())
    {
        unsigned subExprConstIdx = subExpression->GetConstantValueIndex();

        if (exprType->IsBool())
        {
            if (castType->IsBool())
            {
                builtInFunctionCallExpression->SetConstantValueIndex(subExprConstIdx);
            }
            else if (castType->IsInt())
            {
                bool subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

                int64_t value = subValue ? 1 : 0;
                unsigned idx = compilerContext.AddIntConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsFloat())
            {
                bool subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

                double value = subValue ? 1.0 : 0.0;
                unsigned idx = compilerContext.AddFloatConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
        }
        else if (exprType->IsInt())
        {
            int64_t subValue = compilerContext.GetIntConstantValue(subExprConstIdx);

            if (castType->IsBool())
            {
                bool value = subValue != 0;
                unsigned idx = compilerContext.AddBoolConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
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
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsFloat())
            {
                unsigned castSize = castType->GetNumBits();
                double value = 0.0;
                if (castSize == 32)
                {
                    float floatValue = static_cast<float>(subValue);
                    value = static_cast<double>(floatValue);
                }
                else
                {
                    value = static_cast<double>(subValue);
                }

                unsigned idx = compilerContext.AddFloatConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
        }
        else if (exprType->IsFloat())
        {
            double subValue = compilerContext.GetFloatConstantValue(subExprConstIdx);

            if (castType->IsBool())
            {
                bool value = (subValue != 0.0);
                unsigned idx = compilerContext.AddBoolConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsInt())
            {
                int64_t value = 0;
                if (castType->GetSign() == TypeInfo::eSigned)
                {
                    value = static_cast<int64_t>(subValue);
                }
                else
                {
                    uint64_t unsignedValue = static_cast<uint64_t>(subValue);
                    value = static_cast<int64_t>(unsignedValue);
                }

                unsigned castSize = castType->GetNumBits();
                int64_t mask = getBitMask(castSize);
                assert(mask != 0 && "Invalid int type size");
                value &= mask;

                unsigned idx = compilerContext.AddIntConstantValue(value);
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
            else if (castType->IsFloat())
            {
                unsigned idx = subExprConstIdx;
                if (castType->GetNumBits() < exprType->GetNumBits())
                {
                    float floatValue = static_cast<float>(subValue);
                    double value = static_cast<double>(floatValue);
                    idx = compilerContext.AddFloatConstantValue(value);
                }
                builtInFunctionCallExpression->SetConstantValueIndex(idx);
            }
        }
    }
}

void SemanticAnalyzer::BuiltInIntToPtr(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    const Expressions& args = builtInFunctionCallExpression->arguments;
    if (args.size() != 2)
    {
        logger.LogError(*nameToken, "{} expected 2 arguments but got {}", name, args.size());
        isError = true;
        return;
    }

    Expression* typeExpression = args[0];
    const TypeInfo* ptrType = TypeExpressionToType(typeExpression);
    if (ptrType == nullptr)
    {
        isError = true;
        return;
    }

    if (!ptrType->IsPointer())
    {
        StartEndTokenFinder finder;
        typeExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Expected a pointer type but got '{}'",
            ptrType->GetName()
        );
        isError = true;
        return;
    }

    // this can only be called in an 'unchecked' block
    if (uncheckedLevel == 0)
    {
        logger.LogError(*nameToken, "{} may only be called in an 'unchecked' block", name);
        isError = true;
        return;
    }

    Expression* subExpression = args[1];
    subExpression->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* exprType = subExpression->GetType();

    if (!exprType->IsInt() || exprType->GetSign() == TypeInfo::eSigned)
    {
        StartEndTokenFinder finder;
        subExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Cannot cast expression of type '{}' to a pointer. Expected an unsigned integer type",
            exprType->GetName()
        );
        isError = true;
        return;
    }

    builtInFunctionCallExpression->SetType(ptrType);
}

void SemanticAnalyzer::BuiltInPtrToInt(BuiltInFunctionCallExpression* builtInFunctionCallExpression)
{
    const Token* nameToken = builtInFunctionCallExpression->nameToken;
    ROString name = nameToken->value;

    const Expressions& args = builtInFunctionCallExpression->arguments;
    if (args.size() != 1)
    {
        logger.LogError(*nameToken, "{} expected 1 argument but got {}", name, args.size());
        isError = true;
        return;
    }

    Expression* subExpression = args[0];
    subExpression->Accept(this);
    if (isError)
    {
        return;
    }
    const TypeInfo* exprType = subExpression->GetType();

    if (!exprType->IsPointer())
    {
        StartEndTokenFinder finder;
        subExpression->Accept(&finder);

        logger.LogError(
            *finder.start, *finder.end,
            "Cannot cast expression of type '{}' to an integer. Expected a pointer type",
            exprType->GetName()
        );
        isError = true;
        return;
    }

    builtInFunctionCallExpression->SetType(compilerContext.typeRegistry.GetUIntSizeType());
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
    ROString memberName = memberExpression->memberName;
    const MemberInfo* member = type->GetMember(memberName);
    if (member == nullptr)
    {
        const Token* memberToken = memberExpression->memberNameToken;
        logger.LogError(*memberToken, "Type '{}' has no member named '{}'", type->GetName(), memberName);
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
        else if (type->IsStruct())
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
        StartEndTokenFinder finder;
        ifCondition->Accept(&finder);

        isError = true;
        logger.LogError(
            *finder.start, *finder.end,
            "If condition must be a boolean expression"
        );
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
            resultType = GetBiggestSizeType(ifNumLit, elseType, ifSize, elseSize);
            FixNumericLiteralExpression(branchExpression->ifExpression, resultType);
            if (ifSize > elseSize)
            {
                branchExpression->elseExpression = ImplicitCast(branchExpression->elseExpression, resultType);
            }
        }
        else if (!ifIsNumLit && elseIsNumLit)
        {
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
    else if (ifType->IsFloat() && elseType->IsFloat())
    {
        unsigned ifSize = ifType->GetNumBits();
        unsigned elseSize = elseType->GetNumBits();
        bool ifIsLit = ifType->IsLiteral();
        bool elseIsLit = elseType->IsLiteral();

        if (ifIsLit && elseIsLit)
        {
            resultType = GetBiggestSizeType(ifType, elseType);
        }
        else if (ifIsLit && !elseIsLit)
        {
            resultType = GetBiggestSizeType(ifType, elseType);
            FixNumericLiteralExpression(branchExpression->ifExpression, resultType);
            if (ifSize > elseSize)
            {
                branchExpression->elseExpression = ImplicitCast(branchExpression->elseExpression, resultType);
            }
        }
        else if (!ifIsLit && elseIsLit)
        {
            resultType = GetBiggestSizeType(ifType, elseType);
            FixNumericLiteralExpression(branchExpression->elseExpression, resultType);
            if (elseSize > ifSize)
            {
                branchExpression->ifExpression = ImplicitCast(branchExpression->ifExpression, resultType);
            }
        }
        else // if (!ifIsLit && !elseIsLit)
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
            ifType->GetName(),
            elseType->GetName());
        return;
    }

    // set the branch expression's result type
    branchExpression->SetType(resultType);
}

void SemanticAnalyzer::Visit(ConstantDeclaration* constantDeclaration)
{
    ROString constName = constantDeclaration->name;

    // if this constant has already been processed, then we don't need to do anything
    if (unresolvedConsts.find(constName) == unresolvedConsts.end())
    {
        return;
    }

    BinaryExpression* assignmentExpression = constantDeclaration->assignmentExpression;
    if (assignmentExpression->op != BinaryExpression::eAssign)
    {
        isError = true;
        logger.LogInternalError("Binary expression in constant declaration is not an assignment");
        return;
    }

    // if the current const is already in the list of consts being processed,
    // then we've found a recursive dependency
    if (processingConsts.find(constName) != processingConsts.end())
    {
        isError = true;
        logger.LogError(*constantDeclaration->nameToken, "Constant '{}' has a recursive dependency on itself", constName);

        for (auto iter = constIdentifierStack.crbegin(); iter != constIdentifierStack.crend(); ++iter)
        {
            const Token* token = *iter;
            logger.LogNote(*token, "Dependency on '{}'", token->value);
        }

        return;
    }

    Expression* rightExpr = assignmentExpression->right;

    // if this is a struct definition expression, set its name
    StructDefinitionExpression* structDefExpr = dynamic_cast<StructDefinitionExpression*>(rightExpr);
    if (structDefExpr != nullptr)
    {
        structDefExpr->name = constantDeclaration->name;
        structDefExpr->structToken = constantDeclaration->nameToken;
    }

    // process right of assignment expression before adding constant to symbol
    // table in order to detect if the constant is referenced before it is assigned
    vector<StructDefinitionExpression*> incompleteStructExprs;
    incompleteStructExpressions.push(&incompleteStructExprs);
    processingConsts.insert(constName);
    needsStructImplStack.push(false);
    rightExpr->Accept(this);
    needsStructImplStack.pop();
    processingConsts.erase(constName);
    if (isError)
    {
        return;
    }

    if (!rightExpr->GetIsConstant())
    {
        StartEndTokenFinder finder;
        rightExpr->Accept(&finder);

        isError = true;
        logger.LogError(*finder.start, *finder.end, "Assigned expression is not a constant value");
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

    // if this is a new type, register it
    if (type->IsType())
    {
        const TypeInfo* exprType = compilerContext.GetTypeConstantValue(constIdx);

        // create a new name for this type
        const TypeInfo* newType = compilerContext.typeRegistry.GetTypeAlias(constName, constantDeclaration->nameToken, exprType);
        constIdx = compilerContext.AddTypeConstantValue(newType);
    }

    // add the constant name to the symbol table
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

    // process incomplete structs
    processingConsts.insert(constName);
    needsStructImplStack.push(true);
    for (StructDefinitionExpression* structExpr : incompleteStructExprs)
    {
        unsigned idx = structExpr->GetConstantValueIndex();
        if (incompleteStructTypes.find(idx) != incompleteStructTypes.end())
        {
            structExpr->Accept(this);
            if (isError)
            {
                return;
            }
        }
    }
    needsStructImplStack.pop();
    processingConsts.erase(constName);
    incompleteStructExpressions.pop();

    // we've resolved this constant so remove it from the unresolved list
    unresolvedConsts.erase(constName);

    bool isGlobalScope = symbolTable.IsAtGlobalScope();
    if (isGlobalScope)
    {
        orderedGlobalConsts.push_back(constantDeclaration);
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

    const Token* nameToken = variableDeclaration->nameToken;

    // set the variable type
    const TypeInfo* type = GetVariableType(variableDeclaration->typeExpression, rightExpr->GetType(), nameToken);
    if (type == nullptr)
    {
        isError = true;
        return;
    }

    // make sure the type is not 'type', '[]type', '&type', etc.
    const TypeInfo* innerMostType = type;
    while (innerMostType->GetInnerType() != nullptr)
    {
        innerMostType = innerMostType->GetInnerType();
    }
    if (innerMostType->IsType())
    {
        isError = true;
        logger.LogError(*nameToken, "Variable cannot be of type '{}'", type->GetName());
        return;
    }

    variableDeclaration->variableType = type;

    // add the variable name to the symbol table
    ROString varName = variableDeclaration->name;
    bool ok = symbolTable.AddVariable(varName, nameToken, variableDeclaration->variableType);
    if (!ok)
    {
        isError = true;
        LogExistingIdentifierError(varName, nameToken);
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
    unordered_set<ROString> processedParams;

    // set parameter types
    for (Parameter* param : functionDeclaration->parameters)
    {
        ROString paramName = param->name;
        if (processedParams.find(paramName) != processedParams.end())
        {
            const Token* paramToken = param->nameToken;
            logger.LogError(*paramToken, "Function '{}' has multiple parameters named '{}'", functionDeclaration->name, paramName);
            return false;
        }

        const TypeInfo* paramType = TypeExpressionToType(param->typeExpression);
        if (paramType == nullptr)
        {
            return false;
        }

        // make sure the type is not 'type', '[]type', '&type', etc.
        const TypeInfo* innerMostType = paramType;
        while (innerMostType->GetInnerType() != nullptr)
        {
            innerMostType = innerMostType->GetInnerType();
        }
        if (innerMostType->IsType())
        {
            logger.LogError(*param->nameToken, "Parameter cannot be of type '{}'", paramType->GetName());
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
        returnType = TypeExpressionToType(returnTypeExpr);
        if (returnType == nullptr)
        {
            return false;
        }

        // make sure the type is not 'type', '[]type', '&type', etc.
        const TypeInfo* innerMostType = returnType;
        while (innerMostType->GetInnerType() != nullptr)
        {
            innerMostType = innerMostType->GetInnerType();
        }
        if (innerMostType->IsType())
        {
            logger.LogError(*functionDeclaration->nameToken, "Return type cannot be of type '{}'", returnType->GetName());
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
    if (literalType != nullptr || checkType->IsLiteral())
    {
        const char* typeMsg = nullptr;
        if (isArray)
        {
            if (isRange)
            {
                typeMsg = "array of range literals";
            }
            else if (checkType->IsFloat())
            {
                typeMsg = "array of float literals";
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
        else if (checkType->IsFloat())
        {
            typeMsg = "float literals";
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
        type = TypeExpressionToType(typeExpression);
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
        logger.LogError(*errorToken, "Function '{}' has an invalid return type. Expected '{}' but got '{}'", funcDecl->name, returnType->GetName(), expressionType->GetName());
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
    const TypeInfo* exprType = expression->GetType();
    if (exprType->IsOrContainsNumericLiteral() || exprType->IsOrContainsLiteral())
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
