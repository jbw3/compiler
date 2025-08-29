#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "LlvmIrGenerator.h"
#include "CompilerContext.h"
#include "Config.h"
#include "ErrorLogger.h"
#include "SyntaxTree.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Target/TargetMachine.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

namespace fs = std::filesystem;
using namespace llvm;
using namespace std;
using namespace SyntaxTree;

static StringRef toStringRef(ROString str)
{
    return StringRef(str.GetPtr(), str.GetSize());
}

LlvmIrGenerator::DebugScope::DebugScope(
    bool dbgInfoEnabled,
    stack<DIScope*>& scopes,
    DIScope* diScope
) :
    diScopes(scopes),
    dbgInfo(dbgInfoEnabled)
{
    if (dbgInfo)
    {
        diScopes.push(diScope);
    }
}

LlvmIrGenerator::DebugScope::~DebugScope()
{
    if (dbgInfo)
    {
        diScopes.pop();
    }
}

Type* LlvmIrGenerator::strStructElements[STR_STRUCT_ELEMENTS_SIZE];

LlvmIrGenerator::LlvmIrGenerator(CompilerContext& compilerContext) :
    targetMachine(compilerContext.config.targetMachine),
    optimizationLevel(compilerContext.config.optimizationLevel),
    dbgInfo(compilerContext.config.debugInfo),
    boundsCheck(compilerContext.config.boundsCheck),
    compilerContext(compilerContext),
    logger(compilerContext.logger),
    builder(context),
    diBuilder(nullptr),
    module(nullptr),
    symbolTable(compilerContext),
    currentFunction(nullptr),
    resultValue(nullptr),
    unitType(nullptr),
    strStructType(nullptr),
    globalStringCounter(0),
    boolType(Type::getInt1Ty(context))
{
    uIntSizeType = compilerContext.typeRegistry.GetUIntSizeType();
}

LlvmIrGenerator::~LlvmIrGenerator()
{
    delete diBuilder;
}

void LlvmIrGenerator::Visit(SyntaxTree::UnaryExpression* unaryExpression)
{
    Expression* subExpr = unaryExpression->subExpression;

    subExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* subExprValue = resultValue;

    SetDebugLocation(unaryExpression->opToken);

    switch (unaryExpression->op)
    {
        case UnaryExpression::eNegative:
        {
            if (subExpr->GetType()->IsInt())
            {
                resultValue = builder.CreateNeg(subExprValue, "neg");
            }
            else
            {
                resultValue = builder.CreateFNeg(subExprValue, "neg");
            }
            break;
        }
        case UnaryExpression::eComplement:
            resultValue = builder.CreateNot(subExprValue, "not");
            break;
        case UnaryExpression::eAddressOf:
            resultValue = subExprValue;
            break;
        case UnaryExpression::eDereference:
        {
            if (unaryExpression->GetAccessType() == Expression::eAddress)
            {
                resultValue = subExprValue;
            }
            else
            {
                Type* type = CreateLlvmType(unaryExpression->GetType());
                resultValue = builder.CreateLoad(type, subExprValue, "load");
            }
            break;
        }
        case UnaryExpression::eArrayOf:
            resultValue = nullptr;
            logger.LogInternalError("Cannot generate LLVM IR for eArrayOf operator");
            break;
    }
}

void LlvmIrGenerator::Visit(BinaryExpression* binaryExpression)
{
    BinaryExpression::EOperator op = binaryExpression->op;
    Expression* leftExpr = binaryExpression->left;
    Expression* rightExpr = binaryExpression->right;

    if (op == BinaryExpression::eLogicalAnd)
    {
        SetDebugLocation(binaryExpression->opToken);
        resultValue = CreateLogicalBranch(leftExpr, rightExpr, true);
    }
    else if (op == BinaryExpression::eLogicalOr)
    {
        SetDebugLocation(binaryExpression->opToken);
        resultValue = CreateLogicalBranch(leftExpr, rightExpr, false);
    }
    else
    {
        leftExpr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
        Value* leftValue = resultValue;

        rightExpr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
        Value* rightValue = resultValue;

        SetDebugLocation(binaryExpression->opToken);

        bool isAssignment = BinaryExpression::IsAssignment(op);

        // check if values need to be sign extended
        const TypeInfo* leftType = leftExpr->GetType();
        const TypeInfo* rightType = rightExpr->GetType();

        Value* storeValue = nullptr;
        if (isAssignment)
        {
            storeValue = leftValue;

            // if we are also doing a computation (e.g. +=), we need to load the left value
            if (BinaryExpression::IsComputationAssignment(op))
            {
                Type* type = CreateLlvmType(leftType);
                leftValue = builder.CreateLoad(type, leftValue, "load");
            }
        }

        bool isFloat = leftType->IsFloat();
        bool isSigned = leftType->GetSign() == TypeInfo::eSigned;

        switch (op)
        {
            case BinaryExpression::eEqual:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpOEQ(leftValue, rightValue, "cmpeq");
                }
                else
                {
                    resultValue = builder.CreateICmpEQ(leftValue, rightValue, "cmpeq");
                }
                break;
            }
            case BinaryExpression::eNotEqual:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpONE(leftValue, rightValue, "cmpne");
                }
                else
                {
                    resultValue = builder.CreateICmpNE(leftValue, rightValue, "cmpne");
                }
                break;
            }
            case BinaryExpression::eLessThan:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpOLT(leftValue, rightValue, "cmplt");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateICmpSLT(leftValue, rightValue, "cmplt") : builder.CreateICmpULT(leftValue, rightValue, "cmplt");
                }
                break;
            }
            case BinaryExpression::eLessThanOrEqual:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpOLE(leftValue, rightValue, "cmple");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateICmpSLE(leftValue, rightValue, "cmple") : builder.CreateICmpULE(leftValue, rightValue, "cmple");
                }
                break;
            }
            case BinaryExpression::eGreaterThan:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpOGT(leftValue, rightValue, "cmpgt");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateICmpSGT(leftValue, rightValue, "cmpgt") : builder.CreateICmpUGT(leftValue, rightValue, "cmpgt");
                }
                break;
            }
            case BinaryExpression::eGreaterThanOrEqual:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFCmpOGE(leftValue, rightValue, "cmpge");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateICmpSGE(leftValue, rightValue, "cmpge") : builder.CreateICmpUGE(leftValue, rightValue, "cmpge");
                }
                break;
            }
            case BinaryExpression::eAdd:
            case BinaryExpression::eAddAssign:
                resultValue = isFloat ? builder.CreateFAdd(leftValue, rightValue, "add") : builder.CreateAdd(leftValue, rightValue, "add");
                break;
            case BinaryExpression::eSubtract:
            case BinaryExpression::eSubtractAssign:
                resultValue = isFloat ? builder.CreateFSub(leftValue, rightValue, "sub") : builder.CreateSub(leftValue, rightValue, "sub");
                break;
            case BinaryExpression::eMultiply:
            case BinaryExpression::eMultiplyAssign:
                resultValue = isFloat ? builder.CreateFMul(leftValue, rightValue, "mul") : builder.CreateMul(leftValue, rightValue, "mul");
                break;
            case BinaryExpression::eDivide:
            case BinaryExpression::eDivideAssign:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFDiv(leftValue, rightValue, "div");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateSDiv(leftValue, rightValue, "div") : builder.CreateUDiv(leftValue, rightValue, "div");
                }
                break;
            }
            case BinaryExpression::eRemainder:
            case BinaryExpression::eRemainderAssign:
            {
                if (isFloat)
                {
                    resultValue = builder.CreateFRem(leftValue, rightValue, "rem");
                }
                else
                {
                    resultValue = isSigned ? builder.CreateSRem(leftValue, rightValue, "rem") : builder.CreateURem(leftValue, rightValue, "rem");
                }
                break;
            }
            case BinaryExpression::eShiftLeft:
            case BinaryExpression::eShiftLeftAssign:
            case BinaryExpression::eShiftRightLogical:
            case BinaryExpression::eShiftRightLogicalAssign:
            case BinaryExpression::eShiftRightArithmetic:
            case BinaryExpression::eShiftRightArithmeticAssign:
            {
                unsigned leftSize = leftType->GetNumBits();
                unsigned rightSize = rightType->GetNumBits();
                Type* origLeftType = leftValue->getType();
                if (leftSize < rightSize)
                {
                    leftValue = isSigned ?
                        builder.CreateSExt(leftValue, Type::getIntNTy(context, rightSize), "signext") :
                        builder.CreateZExt(leftValue, Type::getIntNTy(context, rightSize), "zeroext");
                }
                else if (leftSize > rightSize)
                {
                    rightValue = builder.CreateZExt(rightValue, Type::getIntNTy(context, leftSize), "zeroext");
                }

                switch (op)
                {
                    case BinaryExpression::eShiftLeft:
                    case BinaryExpression::eShiftLeftAssign:
                        resultValue = builder.CreateShl(leftValue, rightValue, "shl");
                        break;
                    case BinaryExpression::eShiftRightLogical:
                    case BinaryExpression::eShiftRightLogicalAssign:
                        resultValue = builder.CreateLShr(leftValue, rightValue, "lshr");
                        break;
                    case BinaryExpression::eShiftRightArithmetic:
                    case BinaryExpression::eShiftRightArithmeticAssign:
                        resultValue = builder.CreateAShr(leftValue, rightValue, "ashr");
                        break;
                    default:
                        logger.LogInternalError("Unexpected shift operator");
                        resultValue = nullptr;
                        return;
                }

                if (leftSize < rightSize)
                {
                    resultValue = builder.CreateTrunc(resultValue, origLeftType, "trunc");
                }
                break;
            }
            case BinaryExpression::eBitwiseAnd:
            case BinaryExpression::eBitwiseAndAssign:
                resultValue = builder.CreateAnd(leftValue, rightValue, "bitand");
                break;
            case BinaryExpression::eBitwiseXor:
            case BinaryExpression::eBitwiseXorAssign:
                resultValue = builder.CreateXor(leftValue, rightValue, "bitxor");
                break;
            case BinaryExpression::eBitwiseOr:
            case BinaryExpression::eBitwiseOrAssign:
                resultValue = builder.CreateOr(leftValue, rightValue, "bitor");
                break;
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
                logger.LogInternalError("Logical AND and logical OR operators should have been handled before here");
                resultValue = nullptr;
                break;
            case BinaryExpression::eAssign:
                resultValue = rightValue;
                break;
            case BinaryExpression::eClosedRange:
            case BinaryExpression::eHalfOpenRange:
            {
                Type* rangeType = CreateLlvmType(binaryExpression->GetType());
                if (rangeType == nullptr)
                {
                    resultValue = nullptr;
                    logger.LogInternalError("Unknown range type");
                    return;
                }

                vector<unsigned> index(1);
                Value* initValue = UndefValue::get(rangeType);

                // set start
                index[0] = 0;
                initValue = builder.CreateInsertValue(initValue, leftValue, index, "rng");

                // set end
                index[0] = 1;
                initValue = builder.CreateInsertValue(initValue, rightValue, index, "rng");

                resultValue = initValue;
                break;
            }
            case BinaryExpression::eSubscript:
            {
                if (rightType->IsInt())
                {
                    resultValue = GenerateIntSubscriptIr(binaryExpression, leftValue, rightValue);
                }
                else if (rightType->IsRange())
                {
                    resultValue = GenerateRangeSubscriptIr(binaryExpression, leftValue, rightValue);
                }
                else
                {
                    assert(false && "Unexpected subscript type");
                }
                break;
            }
        }

        if (isAssignment)
        {
            builder.CreateStore(resultValue, storeValue);

            // assignment expressions always evaluate to the unit type
            resultValue = ConstantStruct::get(unitType);
        }
    }
}

Value* LlvmIrGenerator::GenerateIntSubscriptIr(const BinaryExpression* binaryExpression, Value* leftValue, Value* rightValue)
{
    const TypeInfo* rightType = binaryExpression->right->GetType();

    unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();

    Value* indexValue = nullptr;
    if (rightType->GetNumBits() < uIntSizeNumBits)
    {
        Type* extType = CreateLlvmType(uIntSizeType);
        indexValue = builder.CreateZExt(rightValue, extType, "zeroext");
    }
    else
    {
        indexValue = rightValue;
    }

    if (boundsCheck)
    {
        vector<uint32_t> sizeIndex;
        sizeIndex.push_back(0);
        Value* size = builder.CreateExtractValue(leftValue, sizeIndex, "size");

        Value* boundsCheck = builder.CreateICmpUGE(indexValue, size, "check");

        Function* function = builder.GetInsertBlock()->getParent();
        BasicBlock* failedBlock = BasicBlock::Create(context, "failed", function);
        BasicBlock* passedBlock = BasicBlock::Create(context, "passed", function);

        builder.CreateCondBr(boundsCheck, failedBlock, passedBlock);

        // generate "failed" block IR
        builder.SetInsertPoint(failedBlock);

        Function* logErrorFunc = module->getFunction("logError");
        if (logErrorFunc != nullptr)
        {
            const Token* opToken = binaryExpression->opToken;

            Type* llvmStrType = CreateLlvmType(compilerContext.typeRegistry.GetStringType());

            const string& filename = compilerContext.GetFilename(opToken->filenameId);
            Constant* fileStrPtr = CreateConstantString(filename);
            Value* fileStr = builder.CreateLoad(llvmStrType, fileStrPtr, "filestr");
            uint64_t line = static_cast<uint64_t>(opToken->line);
            Constant* lineNum = ConstantInt::get(context, APInt(32, line, false));
            Constant* msgStrPtr = CreateConstantString("Index is out of bounds");
            Value* msgStr = builder.CreateLoad(llvmStrType, msgStrPtr, "msgstr");

            vector<Value*> logErrorArgs;
            logErrorArgs.push_back(fileStr);
            logErrorArgs.push_back(lineNum);
            logErrorArgs.push_back(msgStr);
            builder.CreateCall(logErrorFunc, logErrorArgs);
        }

        Function* exitFunc = module->getFunction("exit");

        // declare the function if it does not exist
        if (exitFunc == nullptr)
        {
            vector<Type*> parameters;
            parameters.push_back(Type::getInt32Ty(context));
            FunctionType* funcType = FunctionType::get(Type::getVoidTy(context), parameters, false);
            exitFunc = Function::Create(funcType, Function::ExternalLinkage, "exit", module);
        }

        vector<Value*> exitArgs;
        exitArgs.push_back(ConstantInt::get(context, APInt(32, 1)));
        builder.CreateCall(exitFunc, exitArgs);
        builder.CreateUnreachable();

        // generate "passed" block IR
        builder.SetInsertPoint(passedBlock);
    }

    vector<uint32_t> dataIndex;
    dataIndex.push_back(1);
    Value* data = builder.CreateExtractValue(leftValue, dataIndex, "data");

    vector<Value*> valueIndices;
    valueIndices.push_back(indexValue);
    Type* llvmType = CreateLlvmType(binaryExpression->GetType());
    Value* valuePtr = builder.CreateInBoundsGEP(llvmType, data, valueIndices, "value");
    Value* result = nullptr;
    if (binaryExpression->GetAccessType() == Expression::eAddress)
    {
        result = valuePtr;
    }
    else
    {
        result = builder.CreateLoad(llvmType, valuePtr, "load");
    }

    return result;
}

Value* LlvmIrGenerator::GenerateRangeSubscriptIr(const BinaryExpression* binaryExpression, Value* leftValue, Value* rightValue)
{
    unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();
    Expression* rightExpr = binaryExpression->right;
    const TypeInfo* rightType = rightExpr->GetType();
    const TypeInfo* innerType = rightType->GetInnerType();
    unsigned innerTypeNumBits = innerType->GetNumBits();

    // get array size
    Value* size = builder.CreateExtractValue(leftValue, 0, "size");

    // get array data pointer
    Value* data = builder.CreateExtractValue(leftValue, 1, "data");

    // get range start
    Value* start = builder.CreateExtractValue(rightValue, 0, "start");
    if (innerTypeNumBits < uIntSizeNumBits)
    {
        Type* extType = CreateLlvmType(uIntSizeType);
        start = builder.CreateZExt(start, extType, "zeroext");
    }

    // get range end
    Value* end = builder.CreateExtractValue(rightValue, 1, "end");
    if (innerTypeNumBits < uIntSizeNumBits)
    {
        Type* extType = CreateLlvmType(uIntSizeType);
        end = builder.CreateZExt(end, extType, "zeroext");
    }

    // if this range is closed, add 1 to the end
    if (!rightType->IsHalfOpen())
    {
        Value* one = ConstantInt::get(context, APInt(uIntSizeNumBits, 1, false));
        end = builder.CreateAdd(end, one, "add");
    }

    // check end
    Value* endOk = builder.CreateICmpULT(end, size, "endok");
    Value* checkEnd = builder.CreateSelect(endOk, end, size, "checkend");

    // check start
    Value* startOk = builder.CreateICmpULE(start, checkEnd, "startok");
    Value* checkStart = builder.CreateSelect(startOk, start, checkEnd, "checkstart");

    // calculate new size
    Value* newSize = builder.CreateSub(checkEnd, checkStart, "sub");

    const TypeInfo* resultType = binaryExpression->GetType();
    Type* llvmNewDataType = nullptr;
    if (resultType->IsArray())
    {
        llvmNewDataType = CreateLlvmType(resultType->GetInnerType());
    }
    else if (resultType->IsStr())
    {
        llvmNewDataType = Type::getInt8Ty(context);
    }
    else
    {
        assert(false && "Unexpected subscript type");
    }
    vector<Value*> indices;
    indices.push_back(checkStart);
    Value* newData = builder.CreateInBoundsGEP(llvmNewDataType, data, indices, "ptr");

    // create array struct
    Value* structValue = UndefValue::get(leftValue->getType());
    structValue = builder.CreateInsertValue(structValue, newSize, 0, "agg");
    structValue = builder.CreateInsertValue(structValue, newData, 1, "agg");

    return structValue;
}

void LlvmIrGenerator::Visit(WhileLoop* whileLoop)
{
    Function* function = builder.GetInsertBlock()->getParent();

    BasicBlock* loopCondBlock = BasicBlock::Create(context, "whileCond", function);

    // create unconditional branch from current block to loop condition block
    builder.CreateBr(loopCondBlock);

    // set insert point to the loop condition block
    builder.SetInsertPoint(loopCondBlock);

    // generate the condition IR
    whileLoop->condition->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* conditionResult = resultValue;

    BasicBlock* loopBodyBlock = BasicBlock::Create(context, "whileBody", function);
    BasicBlock* loopExitBlock = BasicBlock::Create(context, "whileExit");

    // create conditional branch from condition block to either the loop body
    // or the loop exit
    builder.CreateCondBr(conditionResult, loopBodyBlock, loopExitBlock);

    // set insert point to the loop body block
    builder.SetInsertPoint(loopBodyBlock);

    // generate loop body IR
    LoopInfo loopInfo;
    loopInfo.breakBlock = loopExitBlock;
    loopInfo.continueBlock = loopCondBlock;
    loops.push(loopInfo);
    whileLoop->expression->Accept(this);
    loops.pop();
    if (resultValue == nullptr)
    {
        return;
    }

    // create unconditional branch to loop condition block
    builder.CreateBr(loopCondBlock);

    // add exit block to function
    loopExitBlock->insertInto(function);

    // set insert point to the loop exit block
    builder.SetInsertPoint(loopExitBlock);

    // while loop expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(ForLoop* forLoop)
{
    Expression* iterExpr = forLoop->iterExpression;
    iterExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* iterableValue = resultValue;

    const TypeInfo* iterableType = iterExpr->GetType();
    bool isRangeLoop = iterableType->IsRange();
    bool isArrayLoop = iterableType->IsArray();
    assert(isRangeLoop || isArrayLoop && "Invalid for loop iterable type");

    const TypeInfo* iterableInnerType = iterableType->GetInnerType();

    // create new scope for iterator
    Scope scope(symbolTable);

    const Token* varNameToken = forLoop->variableNameToken;
    DILexicalBlock* diBlock = nullptr;
    if (dbgInfo)
    {
        // TODO: it would probably be better to use the 'for' token as the scope start
        DIScope* currentScope = diScopes.top();
        unsigned line = varNameToken->line;
        unsigned column = varNameToken->column;
        diBlock = diBuilder->createLexicalBlock(currentScope, currentDiFile, line, column);
    }
    DebugScope dbgScope(dbgInfo, diScopes, diBlock);

    // create iterator
    ROString varName = forLoop->variableName;
    const TypeInfo* varType = forLoop->variableType;
    bool isSigned = varType->GetSign() == TypeInfo::eSigned;
    Type* type = CreateLlvmType(varType);
    assert(type != nullptr && "Unknown variable declaration type");
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, forLoop->variableNameToken, varType, alloca);
    CreateDebugVariable(varNameToken, varType, alloca);

    // create index
    ROString indexName = forLoop->indexName;
    bool hasIndex = indexName.GetSize() > 0;
    const TypeInfo* indexTypeInfo = forLoop->indexType;
    Type* indexType = nullptr;
    AllocaInst* indexAlloca = nullptr;
    if (hasIndex)
    {
        indexType = CreateLlvmType(indexTypeInfo);
        assert(indexType != nullptr && "Unknown variable declaration type");
        indexAlloca = CreateVariableAlloc(currentFunction, indexType, indexName);
        symbolTable.AddVariable(indexName, forLoop->indexNameToken, indexTypeInfo, indexAlloca);
        CreateDebugVariable(forLoop->indexNameToken, indexTypeInfo, indexAlloca);
    }

    Type* iterType = nullptr;
    Value* startValue = nullptr;
    Value* endValue = nullptr;
    Value* data = nullptr;
    if (isRangeLoop)
    {
        iterType = type;

        vector<unsigned> index(1);

        // set iterator to range start
        index[0] = 0;
        startValue = builder.CreateExtractValue(iterableValue, index, "start");
        startValue = CreateExt(startValue, iterableInnerType, varType);

        // get range end
        index[0] = 1;
        endValue = builder.CreateExtractValue(iterableValue, index, "end");
        endValue = CreateExt(endValue, iterableInnerType, varType);
    }
    else if (isArrayLoop)
    {
        iterType = CreateLlvmType(uIntSizeType);

        vector<unsigned> index(1);
        unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();

        // start index is 0
        startValue = ConstantInt::get(context, APInt(uIntSizeNumBits, 0));

        // end index is array size
        index[0] = 0;
        endValue = builder.CreateExtractValue(iterableValue, index, "size");

        // get the array data pointer
        index[0] = 1;
        data = builder.CreateExtractValue(iterableValue, index, "data");
    }

    BasicBlock* incomingBlock = builder.GetInsertBlock();
    Function* function = incomingBlock->getParent();

    BasicBlock* loopCondBlock = BasicBlock::Create(context, "forCond", function);

    // create unconditional branch from current block to loop condition block
    builder.CreateBr(loopCondBlock);

    // set insert point to the loop condition block
    builder.SetInsertPoint(loopCondBlock);

    SetDebugLocation(varNameToken);

    // generate the condition IR
    PHINode* idx = nullptr;
    PHINode* iter = builder.CreatePHI(iterType, 2, "iter");
    iter->addIncoming(startValue, incomingBlock);
    CmpInst::Predicate predicate = CmpInst::BAD_ICMP_PREDICATE;
    if (isRangeLoop)
    {
        if (hasIndex)
        {
            Value* zero = ConstantInt::get(indexType, 0, false);

            idx = builder.CreatePHI(indexType, 2, "idx");
            idx->addIncoming(zero, incomingBlock);
        }

        if (iterableType->IsHalfOpen())
        {
            predicate = isSigned ? CmpInst::ICMP_SLT : CmpInst::ICMP_ULT;
        }
        else
        {
            predicate = isSigned ? CmpInst::ICMP_SLE : CmpInst::ICMP_ULE;
        }
    }
    else if (isArrayLoop)
    {
        predicate = CmpInst::ICMP_ULT;
    }
    Value* conditionResult = builder.CreateICmp(predicate, iter, endValue, "cmp");

    BasicBlock* loopBodyBlock = BasicBlock::Create(context, "forBody", function);
    BasicBlock* loopExitBlock = BasicBlock::Create(context, "forExit");
    BasicBlock* loopIterBlock = BasicBlock::Create(context, "forIter");

    // create conditional branch from condition block to either the loop body
    // or the loop exit
    builder.SetInsertPoint(loopCondBlock);
    builder.CreateCondBr(conditionResult, loopBodyBlock, loopExitBlock);

    // set insert point to the loop body block
    builder.SetInsertPoint(loopBodyBlock);

    if (isRangeLoop)
    {
        // store the iter value in the iterator variable
        builder.CreateStore(iter, alloca);

        if (hasIndex)
        {
            SetDebugLocation(forLoop->indexNameToken);

            // store the index value in the index variable
            builder.CreateStore(idx, indexAlloca);
        }
    }
    else if (isArrayLoop)
    {
        // get the array value and store it in the iterator variable
        vector<Value*> valueIndices;
        valueIndices.push_back(iter);
        Type* llvmIterableInnerType = CreateLlvmType(iterableInnerType);
        Value* valuePtr = builder.CreateInBoundsGEP(llvmIterableInnerType, data, valueIndices, "value");
        Value* value = builder.CreateLoad(llvmIterableInnerType, valuePtr, "load");
        value = CreateExt(value, iterableInnerType, varType);
        builder.CreateStore(value, alloca);

        if (hasIndex)
        {
            SetDebugLocation(forLoop->indexNameToken);

            // zero extend if necessary
            Value* store = nullptr;
            if (indexTypeInfo->GetNumBits() > uIntSizeType->GetNumBits())
            {
                store = builder.CreateZExt(iter, indexType, "zeroext");
            }
            else
            {
                store = iter;
            }

            // store the index value in the index variable
            builder.CreateStore(store, indexAlloca);
        }
    }

    // generate loop body IR
    LoopInfo loopInfo;
    loopInfo.breakBlock = loopExitBlock;
    loopInfo.continueBlock = loopIterBlock;
    loops.push(loopInfo);
    forLoop->expression->Accept(this);
    loops.pop();
    if (resultValue == nullptr)
    {
        return;
    }

    // jump to the iterator update block
    builder.CreateBr(loopIterBlock);

    // add iterator update block to function
    loopIterBlock->insertInto(function);

    // set insert point to the iterator update block
    builder.SetInsertPoint(loopIterBlock);

    // increment iterator
    bool oneIsSigned = isRangeLoop ? isSigned : false;
    Value* one = ConstantInt::get(iterType, 1, oneIsSigned);
    Value* inc = builder.CreateAdd(iter, one, "inc");
    iter->addIncoming(inc, loopIterBlock);

    // increment index
    if (hasIndex && isRangeLoop)
    {
        Value* idxOne = ConstantInt::get(indexType, 1, false);
        Value* idxInc = builder.CreateAdd(idx, idxOne, "inc");
        idx->addIncoming(idxInc, loopIterBlock);
    }

    // create unconditional branch to loop condition block
    builder.CreateBr(loopCondBlock);

    // add exit block to function
    loopExitBlock->insertInto(function);

    // set insert point to the loop exit block
    builder.SetInsertPoint(loopExitBlock);

    // for loop expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(LoopControl* loopControl)
{
    const Token* token = loopControl->token;
    Token::EType tokenType = token->type;
    LoopInfo loopInfo = loops.top();

    SetDebugLocation(token);

    if (tokenType == Token::eBreak)
    {
        builder.CreateBr(loopInfo.breakBlock);
    }
    else if (tokenType == Token::eContinue)
    {
        builder.CreateBr(loopInfo.continueBlock);
    }
    else
    {
        assert(false && "Unknown control type");
    }

    // need to create a new basic block for any following instructions
    // because we just terminated the current one with a branch
    stringstream ss;
    ss << "after" << loopControl->token->value;
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* newBlock = BasicBlock::Create(context, ss.str(), function);
    builder.SetInsertPoint(newBlock);

    // return expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(Return* ret)
{
    SetDebugLocation(ret->token);

    ret->expression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    builder.CreateRet(resultValue);

    // need to create a new basic block for any following instructions
    // because we just terminated the current one with a return
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* newBlock = BasicBlock::Create(context, "afterreturn", function);
    builder.SetInsertPoint(newBlock);

    // return expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(FunctionTypeExpression* /*functionTypeExpression*/)
{
    resultValue = nullptr;
    logger.LogInternalError("Cannot generate LLVM IR for FunctionTypeExpression");
}

void LlvmIrGenerator::Visit(ExternFunctionDeclaration* /*externFunctionDeclaration*/)
{
    // nothing to do here
}

void LlvmIrGenerator::Visit(FunctionDefinition* functionDefinition)
{
    const FunctionDeclaration* declaration = functionDefinition->declaration;
    ROString funcName = declaration->name;
    Function* func = module->getFunction(toStringRef(funcName));
    if (func == nullptr)
    {
        logger.LogInternalError("Function '{}' was not declared", funcName);
        resultValue = nullptr;
        return;
    }

    if (!func->empty())
    {
        logger.LogInternalError("Cannot redefine function '{}'", funcName);
        resultValue = nullptr;
        return;
    }

    // if we're not optimizing, set these attributes so the assembly generator
    // will not optimize (it also speeds up assembly generation)
    if (optimizationLevel == 0)
    {
        func->addFnAttr(Attribute::NoInline);
        func->addFnAttr(Attribute::OptimizeNone);
    }

    // this language doesn't have exceptions, so no functions need to support unwinding
    func->addFnAttr(Attribute::NoUnwind);

    // create entry block
    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(basicBlock);

    Scope scope(symbolTable);

    const Parameters& params = declaration->parameters;

    DISubprogram* diSubprogram = nullptr;
    if (dbgInfo)
    {
        SmallVector<Metadata*, 8> funTypes;
        DIType* retDebugType = CreateLlvmDebugType(declaration->returnType);
        if (retDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        funTypes.push_back(retDebugType);

        for (const Parameter* param : params)
        {
            DIType* paramDebugType = CreateLlvmDebugType(param->type);
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            funTypes.push_back(paramDebugType);
        }

        DISubroutineType* subroutine = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(funTypes), DINode::FlagPrototyped);

        unsigned line = declaration->nameToken->line;
        diSubprogram = diBuilder->createFunction(currentDiFile, toStringRef(funcName), "", currentDiFile, line, subroutine, 0, DINode::FlagZero, DISubprogram::SPFlagDefinition);
        func->setSubprogram(diSubprogram);

        // unset debug location for function prolog
        builder.SetCurrentDebugLocation(DebugLoc());
    }

    DebugScope dbgScope(dbgInfo, diScopes, diSubprogram);

    unsigned idx = 0;
    for (Argument& arg : func->args())
    {
        const Parameter* param = params[idx];
        ROString paramName = param->name;
        arg.setName(toStringRef(paramName));
        AllocaInst* alloca = CreateVariableAlloc(func, arg.getType(), paramName);
        builder.CreateStore(&arg, alloca);
        const TypeInfo* paramType = param->type;
        const Token* token = param->nameToken;
        symbolTable.AddVariable(paramName, token, paramType, alloca);

        if (dbgInfo)
        {
            unsigned line = token->line;
            DIType* paramDebugType = CreateLlvmDebugType(paramType);
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            DILocalVariable* diVar = diBuilder->createParameterVariable(diSubprogram, toStringRef(paramName), idx + 1, currentDiFile, line, paramDebugType, true);
            diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DILocation::get(context, line, 0, diSubprogram), builder.GetInsertBlock());
        }

        ++idx;
    }

    // process function body expression
    currentFunctionDefinition = functionDefinition;
    currentFunction = func;
    Expression* expression = functionDefinition->expression;
    expression->Accept(this);
    currentFunction = nullptr;
    currentFunctionDefinition = nullptr;

    if (resultValue == nullptr)
    {
        return;
    }

    // if the function ends with a return statement, there will be an empty
    // block at the end that we need to remove
    if (functionDefinition->endsWithReturnStatement)
    {
        BasicBlock& lastBlock = func->back();
        assert(lastBlock.empty() && "Expected function's last basic block to be empty");
        lastBlock.eraseFromParent();
    }
    else // the function does not end with a return statement, so return the last expression
    {
        builder.CreateRet(resultValue);
    }

    if (dbgInfo)
    {
        // have to finalize subprogram before verifyFunction() is called
        diBuilder->finalizeSubprogram(diSubprogram);
    }

#ifndef NDEBUG
    bool error = verifyFunction(*func, &errs());
    if (error)
    {
        resultValue = nullptr;
        logger.LogInternalError("Verify function failed");
        return;
    }
#endif // NDEBUG
}

void LlvmIrGenerator::Visit(StructDefinition* structDefinition)
{
    ROString structName = structDefinition->name;
    const TypeInfo* typeInfo = structDefinition->type;

    vector<Type*> members;
    for (const MemberDefinition* memberDef : structDefinition->members)
    {
        const MemberInfo* memberInfo = typeInfo->GetMember(memberDef->name);
        Type* memberType = CreateLlvmType(memberInfo->GetType());
        if (memberType == nullptr)
        {
            resultValue = nullptr;
            logger.LogInternalError("Unknown member definition type");
            return;
        }

        members.push_back(memberType);
    }

    auto iter = types.find(typeInfo->GetId());
    assert(iter != types.end());
    StructType* structType = static_cast<StructType*>(iter->second);
    structType->setBody(members);

    if (dbgInfo)
    {
        DIFile* file = diFiles[structDefinition->fileId];

        SmallVector<Metadata*, 8> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : typeInfo->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = CreateLlvmDebugType(memberType);
            if (memberDiType == nullptr)
            {
                resultValue = nullptr;
                return;
            }

            ROString memberName = member->GetName();
            uint64_t memberSize = memberDiType->getSizeInBits();
            // TODO: better way to get alignment?
            uint32_t alignment = (memberSize > 32) ? 32 : static_cast<uint32_t>(memberSize);
            unsigned memberLine = member->GetToken()->line;
            elements.push_back(diBuilder->createMemberType(file, toStringRef(memberName), file, memberLine, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

            offset += memberSize;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        auto iter = diStructTypes.find(structName);
        assert(iter != diStructTypes.end());
        DICompositeType* diType = iter->second;
        diType->replaceElements(elementsArray);
    }
}

void LlvmIrGenerator::Visit(StructDefinitionExpression* structDefinitionExpression)
{
    unsigned typeIdx = structDefinitionExpression->GetConstantValueIndex();
    const TypeInfo* structType = compilerContext.GetTypeConstantValue(typeIdx);
    ROString structName = structType->GetShortName();

    // TODO: delete this?
#if 0
    // add debug info for struct members
    if (dbgInfo)
    {
        DIFile* diFile = diFiles[structDefinitionExpression->fileId];

        SmallVector<Metadata*, 8> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : structType->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = CreateLlvmDebugType(memberType);
            if (memberDiType == nullptr)
            {
                resultValue = nullptr;
                return;
            }

            uint64_t memberSize = memberDiType->getSizeInBits();
            unsigned pointerSize = compilerContext.typeRegistry.GetPointerSize();
            uint32_t alignment = (memberSize > pointerSize) ? pointerSize : static_cast<uint32_t>(memberSize);
            if (alignment > 0)
            {
                uint32_t rem = offset % alignment;
                if (rem > 0)
                {
                    offset += alignment - rem;
                }
            }

            ROString memberName = member->GetName();
            unsigned memberLine = member->GetToken()->line;
            elements.push_back(diBuilder->createMemberType(diFile, toStringRef(memberName), diFile, memberLine, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

            offset += memberSize;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        auto iter = diStructTypes.find(structName);
        assert(iter != diStructTypes.end());
        DICompositeType* diType = iter->second;
        diType->replaceElements(elementsArray);
    }
#endif
}

void LlvmIrGenerator::Visit(StructInitializationExpression* structInitializationExpression)
{
    const TypeInfo* typeInfo = structInitializationExpression->GetType();
    Type* type = CreateLlvmType(typeInfo);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown struct init type");
        return;
    }

    vector<unsigned> index(1);
    Value* initValue = UndefValue::get(type);
    for (const MemberInitialization* member : structInitializationExpression->memberInitializations)
    {
        Expression* expr = member->expression;
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        SetDebugLocation(member->nameToken);

        const MemberInfo* memberInfo = typeInfo->GetMember(member->name);

        index[0] = memberInfo->GetIndex();
        initValue = builder.CreateInsertValue(initValue, resultValue, index, "agg");
    }

    resultValue = initValue;
}

void LlvmIrGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    // generate code for functions
    for (FunctionDefinition* funcDef : moduleDefinition->functionDefinitions)
    {
        funcDef->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
    }
}

void LlvmIrGenerator::Visit(Modules* modules)
{
    // add all struct names to types map
    for (StructDefinition* structDef : modules->orderedStructDefinitions)
    {
        ROString structName = structDef->name;
        StructType* structType = StructType::create(context, toStringRef(structName));
        types.insert({structDef->type->GetId(), structType});

        // add debug info for structs
        if (dbgInfo)
        {
            DIFile* diFile = diFiles[structDef->fileId];

            const TypeInfo* structDefType = structDef->type;

            ROString name = structDefType->GetShortName();
            unsigned numBits = structDefType->GetNumBits();

            unsigned line = structDefType->GetToken()->line;
            // TODO: set alignment
            SmallVector<Metadata*, 0> elements;
            DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
            DICompositeType* diType = diBuilder->createStructType(diFile, toStringRef(name), diFile, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
            diStructTypes.insert({structDef->name, diType});
        }
    }

    // TODO: is this needed?
#if 0
    // add all struct names to types map
    for (const ModuleDefinition* moduleDefinition : modules->modules)
    {
        for (const ConstantDeclaration* constDecl : moduleDefinition->constantDeclarations)
        {
            const Expression* expr = constDecl->assignmentExpression->right;
            const StructDefinitionExpression* structDef = dynamic_cast<const StructDefinitionExpression*>(expr);
            if (structDef != nullptr)
            {
                unsigned typeIdx = structDef->GetConstantValueIndex();
                const TypeInfo* exprType = compilerContext.GetTypeConstantValue(typeIdx);
                ROString structName = exprType->GetShortName();

                if (exprType->IsAggregate())
                {
                    StructType* structType = StructType::create(context, toStringRef(structName));
                    types.insert({exprType->GetId(), structType});

                    // add debug info for structs
                    if (dbgInfo)
                    {
                        DIFile* diFile = diFiles[structDef->fileId];

                        ROString name = exprType->GetShortName();
                        unsigned numBits = exprType->GetNumBits();

                        unsigned line = constDecl->nameToken->line;
                        // TODO: set alignment
                        SmallVector<Metadata*, 0> elements;
                        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
                        DICompositeType* diType = diBuilder->createStructType(diFile, toStringRef(name), diFile, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
                        diStructTypes.insert({structName, diType});
                    }
                }
            }
            else if (constDecl->constantType->IsType())
            {
                // TODO: add debug type info for struct alias
                // unsigned typeIdx = expr->GetConstantValueIndex();
                // const TypeInfo* exprType = compilerContext.GetTypeConstantValue(typeIdx);
                // Type* llvmType = GetType(exprType);
                // assert(llvmType != nullptr && "Could not find LLVM type");
                // types.insert({constDecl->name, llvmType});
            }
        }
    }
#endif

    // add global constants to symbol table
    for (ConstantDeclaration* constDecl : modules->orderedGlobalConstants)
    {
        constDecl->Accept(this);
    }

    // generate struct declarations
    for (StructDefinition* structDef : modules->orderedStructDefinitions)
    {
        structDef->Accept(this);
    }

    // create function declarations

    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        for (ExternFunctionDeclaration* externFunc : moduleDefinition->externFunctionDeclarations)
        {
            const FunctionDeclaration* decl = externFunc->declaration;
            bool ok = CreateFunctionDeclaration(decl);
            if (!ok)
            {
                resultValue = nullptr;
                return;
            }

            symbolTable.AddConstant(decl->name, decl->nameToken, externFunc->GetType(), externFunc->GetConstantValueIndex());
        }

        for (FunctionDefinition* funcDef : moduleDefinition->functionDefinitions)
        {
            const FunctionDeclaration* decl = funcDef->declaration;
            bool ok = CreateFunctionDeclaration(decl);
            if (!ok)
            {
                resultValue = nullptr;
                return;
            }

            symbolTable.AddConstant(decl->name, decl->nameToken, funcDef->GetType(), funcDef->GetConstantValueIndex());
        }
    }

    if (dbgInfo)
    {
        DIFile* diFile = diFiles[0];
        bool isOptimized = optimizationLevel > 0;
        diBuilder->createCompileUnit(dwarf::DW_LANG_C, diFile, "WIP Compiler", isOptimized, "", 0);
    }

    for (ModuleDefinition* moduleDefinition : modules->modules)
    {
        if (dbgInfo)
        {
            unsigned fileId = moduleDefinition->fileId;
            currentDiFile = diFiles[fileId];
            diScopes.push(currentDiFile);
        }

        moduleDefinition->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        if (dbgInfo)
        {
            diScopes.pop();
        }
    }
}

void LlvmIrGenerator::Visit(UnitTypeLiteralExpression* /* unitTypeLiteralExpression */)
{
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(NumericExpression* numericExpression)
{
    unsigned numBits = 0;
    bool isSigned = false;

    const TypeInfo* type = numericExpression->GetType();
    const NumericLiteralType* numLitType = dynamic_cast<const NumericLiteralType*>(type);
    TypeInfo::ESign sign = type->GetSign();
    if (sign == TypeInfo::eSigned)
    {
        numBits = (numLitType != nullptr) ? numLitType->GetSignedNumBits() : type->GetNumBits();
        isSigned = true;
    }
    else if (sign == TypeInfo::eUnsigned || sign == TypeInfo::eContextDependent)
    {
        numBits = (numLitType != nullptr) ? numLitType->GetUnsignedNumBits() : type->GetNumBits();
        isSigned = false;
    }
    else
    {
        logger.LogInternalError("Unexpected numeric expression sign");
        resultValue = nullptr;
        return;
    }

    SetDebugLocation(numericExpression->token);

    int64_t value = numericExpression->value;
    resultValue = ConstantInt::get(context, APInt(numBits, value, isSigned));
}

void LlvmIrGenerator::Visit(FloatLiteralExpression* floatLiteralExpression)
{
    SetDebugLocation(floatLiteralExpression->token);

    double doubleValue = floatLiteralExpression->value;

    unsigned numBits = floatLiteralExpression->GetType()->GetNumBits();
    if (numBits == 32)
    {
        float singleValue = static_cast<float>(doubleValue);
        resultValue = ConstantFP::get(context, APFloat(singleValue));
    }
    else if (numBits == 64)
    {
        resultValue = ConstantFP::get(context, APFloat(doubleValue));
    }
    else
    {
        logger.LogInternalError("Unexpected float size");
        resultValue = nullptr;
    }
}

void LlvmIrGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    const Token* token = boolLiteralExpression->token;

    SetDebugLocation(token);

    bool value = token->type == Token::eTrueLit;
    resultValue = value ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
}

void LlvmIrGenerator::Visit(StringLiteralExpression* stringLiteralExpression)
{
    const vector<char>& chars = stringLiteralExpression->characters;
    Constant* strPtr = CreateConstantString(chars);
    Type* type = CreateLlvmType(compilerContext.typeRegistry.GetStringType());
    resultValue = builder.CreateLoad(type, strPtr, "load");
}

Constant* LlvmIrGenerator::CreateConstantString(const string& str)
{
    vector<char> chars;
    for (char ch : str)
    {
        chars.push_back(ch);
    }

    return CreateConstantString(chars);
}

Constant* LlvmIrGenerator::CreateConstantString(const vector<char>& chars)
{
    Constant* globalString = nullptr;

    // if we already have a constant for this string, then reuse it
    auto iter = strings.find(chars);
    if (iter != strings.end())
    {
        globalString = iter->second;
    }
    else // create a new string constant
    {
        size_t numChars = chars.size();
        Constant* charsArray = ConstantDataArray::getString(context, StringRef(chars.data(), numChars), false);

        // create names
        stringstream dataName;
        dataName << "strData" << globalStringCounter;
        stringstream structName;
        structName << "strStruct" << globalStringCounter;
        ++globalStringCounter;

        module->getOrInsertGlobal(dataName.str(), charsArray->getType());
        GlobalVariable* strData = module->getNamedGlobal(dataName.str());
        strData->setConstant(true);
        strData->setInitializer(charsArray);

        Constant* strDataPointer = ConstantExpr::getBitCast(strData, strStructType->getElementType(1));

        vector<Constant*> initValues =
        {
            ConstantInt::get(context, APInt(uIntSizeType->GetNumBits(), numChars, false)),
            strDataPointer,
        };
        Constant* initializer = ConstantStruct::get(strStructType, initValues);

        module->getOrInsertGlobal(structName.str(), initializer->getType());
        GlobalVariable* globalStruct = module->getNamedGlobal(structName.str());
        globalStruct->setConstant(true);
        globalStruct->setInitializer(initializer);

        strings[chars] = globalStruct;

        globalString = globalStruct;
    }

    return globalString;
}

Value* LlvmIrGenerator::CreateConstantValue(const TypeInfo* type, unsigned constIdx)
{
    Value* constValue = nullptr;

    if (type->IsBool())
    {
        bool value = compilerContext.GetBoolConstantValue(constIdx);
        constValue = value ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
    }
    else if (type->IsInt())
    {
        int64_t value = compilerContext.GetIntConstantValue(constIdx);
        unsigned numBits = type->GetNumBits();
        bool isSigned = type->GetSign() == TypeInfo::eSigned;
        constValue = ConstantInt::get(context, APInt(numBits, value, isSigned));
    }
    else if (type->IsFloat())
    {
        double doubleValue = compilerContext.GetFloatConstantValue(constIdx);
        unsigned numBits = type->GetNumBits();
        if (numBits == 32)
        {
            float singleValue = static_cast<float>(doubleValue);
            constValue = ConstantFP::get(context, APFloat(singleValue));
        }
        else if (numBits == 64)
        {
            constValue = ConstantFP::get(context, APFloat(doubleValue));
        }
        else
        {
            logger.LogInternalError("Unexpected float size");
            return nullptr;
        }
    }
    else if (type->IsStr())
    {
        const vector<char>& value = compilerContext.GetStrConstantValue(constIdx);
        Constant* strPtr = CreateConstantString(value);
        Type* llvmStrType = CreateLlvmType(compilerContext.typeRegistry.GetStringType());
        constValue = builder.CreateLoad(llvmStrType, strPtr, "load");
    }
    else if (type->IsRange())
    {
        StructType* rangeType = static_cast<StructType*>(CreateLlvmType(type));
        if (rangeType == nullptr)
        {
            logger.LogInternalError("Unknown range type");
            return nullptr;
        }

        const RangeConstValue& value = compilerContext.GetRangeConstantValue(constIdx);

        const TypeInfo* innerType = type->GetInnerType();
        unsigned numBits = innerType->GetNumBits();
        bool isSigned = innerType->GetSign() == TypeInfo::eSigned;

        Constant* startValue = ConstantInt::get(context, APInt(numBits, value.start, isSigned));
        Constant* endValue = ConstantInt::get(context, APInt(numBits, value.end, isSigned));
        vector<Constant*> initValues = { startValue, endValue };

        constValue = ConstantStruct::get(rangeType, initValues);
    }
    else if (type->IsAggregate())
    {
        Type* llvmType = CreateLlvmType(type);
        if (llvmType == nullptr)
        {
            logger.LogInternalError("Unknown const struct definition type");
            return nullptr;
        }

        const vector<const MemberInfo*>& members = type->GetMembers();
        const StructConstValue& value = compilerContext.GetStructConstantValue(constIdx);
        if (members.size() != value.memberIndices.size())
        {
            logger.LogInternalError("Member size mismatch for const struct");
            return nullptr;
        }

        vector<unsigned> index(1);
        Value* llvmValue = UndefValue::get(llvmType);
        unsigned memberIdx = 0;
        for (unsigned memberConstIdx : value.memberIndices)
        {
            const TypeInfo* memberType = members[memberIdx]->GetType();
            Value* memberValue = CreateConstantValue(memberType, memberConstIdx);

            index[0] = memberIdx;
            llvmValue = builder.CreateInsertValue(llvmValue, memberValue, index, "agg");
            ++memberIdx;
        }

        constValue = llvmValue;
    }
    else if (type->IsArray())
    {
        const ArrayConstValue& value = compilerContext.GetArrayConstantValue(constIdx);

        if (value.type == ArrayConstValue::eSizeValue)
        {
            unsigned sizeConstIdx = value.valueIndices[0];
            unsigned valueConstIdx = value.valueIndices[1];

            int64_t sizeConstValue = compilerContext.GetIntConstantValue(sizeConstIdx);
            uint64_t arraySize = static_cast<uint64_t>(sizeConstValue);

            Value* arrayValue = CreateConstantValue(type->GetInnerType(), valueConstIdx);

            constValue = CreateSizeValueArrayIr(type, arraySize, arrayValue);
        }
        else if (value.type == ArrayConstValue::eMultiValue)
        {
            uint64_t arraySize = value.valueIndices.size();
            const TypeInfo* elementType = type->GetInnerType();

            Type* arrayType = CreateLlvmType(type);
            Type* innerType = CreateLlvmType(elementType);
            Type* llvmArrayType = ArrayType::get(innerType, arraySize);
            AllocaInst* alloca = CreateVariableAlloc(currentFunction, llvmArrayType, "array");

            unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();
            for (uint64_t i = 0; i < arraySize; ++i)
            {
                unsigned elementConstIdx = value.valueIndices[i];
                Value* elementValue = CreateConstantValue(elementType, elementConstIdx);

                vector<Value*> indices;
                indices.push_back(ConstantInt::get(context, APInt(uIntSizeNumBits, 0)));
                indices.push_back(ConstantInt::get(context, APInt(uIntSizeNumBits, i)));

                Value* ptr = builder.CreateInBoundsGEP(alloca->getAllocatedType(), alloca, indices, "ptr");
                builder.CreateStore(elementValue, ptr);
            }

            // create array struct
            Value* ptrValue = builder.CreateBitCast(alloca, arrayType->getStructElementType(1), "arrptr");
            Value* structValue = UndefValue::get(arrayType);
            Value* sizeValue = ConstantInt::get(context, APInt(uIntSizeNumBits, arraySize));
            structValue = builder.CreateInsertValue(structValue, sizeValue, 0, "agg");
            structValue = builder.CreateInsertValue(structValue, ptrValue, 1, "agg");

            constValue = structValue;
        }
        else
        {
            logger.LogInternalError("Unexpected constant array type '{}'", value.type);
        }
    }
    else if (type->IsFunction())
    {
        const FunctionDeclaration* decl = compilerContext.GetFunctionConstantValue(constIdx);
        Function* func = module->getFunction(toStringRef(decl->name));
        constValue = func;
    }
    else
    {
        logger.LogInternalError("Unexpected constant type '{}'", type->GetShortName());
    }

    return constValue;
}

void LlvmIrGenerator::Visit(IdentifierExpression* identifierExpression)
{
    ROString name = identifierExpression->name;
    const SymbolTable::IdentifierData* data = symbolTable.GetIdentifierData(name);
    if (data == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("No identifier data found for '{}'", name);
        return;
    }

    SetDebugLocation(identifierExpression->token);

    if (data->IsConstant())
    {
        unsigned constIdx = data->constValueIndex;
        const TypeInfo* type = identifierExpression->GetType();
        resultValue = CreateConstantValue(type, constIdx);
        if (resultValue == nullptr)
        {
            return;
        }
    }
    else
    {
        AllocaInst* alloca = data->value;
        Expression::EAccessType accessType = identifierExpression->GetAccessType();
        switch (accessType)
        {
            case Expression::eValue:
                resultValue = builder.CreateLoad(alloca->getAllocatedType(), alloca, toStringRef(name));
                break;
            case Expression::eAddress:
                resultValue = alloca;
                break;
        }
    }
}

void LlvmIrGenerator::Visit(ArraySizeValueExpression* arrayExpression)
{
    Expression* sizeExpression = arrayExpression->sizeExpression;
    unsigned constIdx = sizeExpression->GetConstantValueIndex();
    int64_t constValue = compilerContext.GetIntConstantValue(constIdx);
    uint64_t arraySize = static_cast<uint64_t>(constValue);

    Expression* valueExpression = arrayExpression->valueExpression;
    valueExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* valueValue = resultValue;

    resultValue = CreateSizeValueArrayIr(arrayExpression->GetType(), arraySize, valueValue);
}

llvm::Value* LlvmIrGenerator::CreateSizeValueArrayIr(const TypeInfo* arrayTypeInfo, uint64_t arraySize, Value* arrayValue)
{
    unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();

    const TypeInfo* innerTypeInfo = arrayTypeInfo->GetInnerType();
    Type* arrayType = CreateLlvmType(arrayTypeInfo);
    Type* innerType = CreateLlvmType(innerTypeInfo);
    Type* llvmArrayType = ArrayType::get(innerType, arraySize);
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, llvmArrayType, "array");

    Value* sizeValue = ConstantInt::get(context, APInt(uIntSizeNumBits, arraySize));

    // create loop to insert values
    if (arraySize > 0)
    {
        Value* zero = ConstantInt::get(context, APInt(uIntSizeNumBits, 0));
        Value* one = ConstantInt::get(context, APInt(uIntSizeNumBits, 1));

        BasicBlock* preLoopBlock = builder.GetInsertBlock();
        BasicBlock* loopBodyBlock = BasicBlock::Create(context, "fillBody", currentFunction);
        BasicBlock* loopExitBlock = BasicBlock::Create(context, "fillExit");

        vector<Value*> indices(2);
        indices[0] = zero;

        // get address of first element
        indices[1] = zero;
        Value* startPtr = builder.CreateInBoundsGEP(llvmArrayType, alloca, indices, "startPtr");

        // get address of end (1 past the last element)
        indices[1] = sizeValue;
        Value* endPtr = builder.CreateInBoundsGEP(llvmArrayType, alloca, indices, "endPtr");

        // branch to loop body block
        builder.CreateBr(loopBodyBlock);

        // set insert point to the loop body block
        builder.SetInsertPoint(loopBodyBlock);

        PHINode* phi = builder.CreatePHI(startPtr->getType(), 2, "phi");
        phi->addIncoming(startPtr, preLoopBlock);

        // store the value
        builder.CreateStore(arrayValue, phi);

        // increment the address
        Value* nextPtr = builder.CreateInBoundsGEP(llvmArrayType, phi, one, "nextPtr");
        phi->addIncoming(nextPtr, loopBodyBlock);

        // check if we've reached the end
        Value* atEnd = builder.CreateICmpEQ(nextPtr, endPtr, "atEnd");
        builder.CreateCondBr(atEnd, loopExitBlock, loopBodyBlock);

        // add exit block to function
        loopExitBlock->insertInto(currentFunction);

        // set insert point to the loop exit block
        builder.SetInsertPoint(loopExitBlock);
    }

    // create array struct
    Value* ptrValue = builder.CreateBitCast(alloca, arrayType->getStructElementType(1), "arrptr");
    Value* structValue = UndefValue::get(arrayType);
    structValue = builder.CreateInsertValue(structValue, sizeValue, 0, "agg");
    structValue = builder.CreateInsertValue(structValue, ptrValue, 1, "agg");

    return structValue;
}

void LlvmIrGenerator::Visit(ArrayMultiValueExpression* arrayExpression)
{
    const Expressions& expressions = arrayExpression->expressions;
    uint64_t arraySize = expressions.size();

    const TypeInfo* typeInfo = arrayExpression->GetType();
    Type* arrayType = CreateLlvmType(typeInfo);
    Type* innerType = CreateLlvmType(typeInfo->GetInnerType());
    Type* llvmArrayType = ArrayType::get(innerType, arraySize);
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, llvmArrayType, "array");

    unsigned uIntSizeNumBits = uIntSizeType->GetNumBits();
    for (uint64_t i = 0; i < arraySize; ++i)
    {
        Expression* expr = expressions[i];
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        vector<Value*> indices;
        indices.push_back(ConstantInt::get(context, APInt(uIntSizeNumBits, 0)));
        indices.push_back(ConstantInt::get(context, APInt(uIntSizeNumBits, i)));

        Value* ptr = builder.CreateInBoundsGEP(alloca->getAllocatedType(), alloca, indices, "ptr");
        builder.CreateStore(resultValue, ptr);
    }

    // create array struct
    Value* ptrValue = builder.CreateBitCast(alloca, arrayType->getStructElementType(1), "arrptr");
    Value* structValue = UndefValue::get(arrayType);
    Value* sizeValue = ConstantInt::get(context, APInt(uIntSizeNumBits, arraySize));
    structValue = builder.CreateInsertValue(structValue, sizeValue, 0, "agg");
    structValue = builder.CreateInsertValue(structValue, ptrValue, 1, "agg");

    resultValue = structValue;
}

void LlvmIrGenerator::Visit(BlockExpression* blockExpression)
{
    // create new scope for block
    Scope scope(symbolTable);

    DILexicalBlock* diBlock = nullptr;
    if (dbgInfo)
    {
        DIScope* currentScope = diScopes.top();
        const Token* startToken = blockExpression->startToken;
        unsigned line = startToken->line;
        unsigned column = startToken->column;
        diBlock = diBuilder->createLexicalBlock(currentScope, currentDiFile, line, column);
    }
    DebugScope dbgScope(dbgInfo, diScopes, diBlock);

    for (ConstantDeclaration* constantDeclaration : blockExpression->constantDeclarations)
    {
        constantDeclaration->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }
    }

    const SyntaxTreeNodes& statements = blockExpression->statements;
    size_t size = statements.size();

    if (size == 0)
    {
        resultValue = nullptr;
        logger.LogInternalError("Block expression has no statements");
    }
    else
    {
        for (SyntaxTreeNode* statement : statements)
        {
            statement->Accept(this);
            if (resultValue == nullptr)
            {
                break;
            }
        }

        // the block expression's result is the result of its last expression
    }
}

void LlvmIrGenerator::Visit(CastExpression* castExpression)
{
    Expression* subExpression = castExpression->subExpression;
    subExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    SetDebugLocation(castExpression->castToken);

    const TypeInfo* exprType = subExpression->GetType();
    const TypeInfo* castType = castExpression->GetType();

    if (exprType->IsBool())
    {
        if (castType->IsInt())
        {
            Type* dstType = CreateLlvmType(castType);
            resultValue = builder.CreateZExt(resultValue, dstType, "cast");
        }
        else if (castType->IsFloat())
        {
            Type* dstType = CreateLlvmType(castType);
            resultValue = builder.CreateUIToFP(resultValue, dstType, "cast");
        }
        else if (castType->IsBool())
        {
            // nothing to do in this case
        }
        else
        {
            assert(false && "Invalid cast");
        }
    }
    else if (exprType->IsInt())
    {
        if (castType->IsInt())
        {
            unsigned exprSize = exprType->GetNumBits();
            unsigned castSize = castType->GetNumBits();
            if (castSize < exprSize)
            {
                Type* dstType = CreateLlvmType(castType);
                resultValue = builder.CreateTrunc(resultValue, dstType, "cast");
            }
            else if (castSize > exprSize)
            {
                Type* dstType = CreateLlvmType(castType);

                TypeInfo::ESign exprSign = exprType->GetSign();
                if (exprSign == TypeInfo::eSigned)
                {
                    resultValue = builder.CreateSExt(resultValue, dstType, "cast");
                }
                else
                {
                    resultValue = builder.CreateZExt(resultValue, dstType, "cast");
                }
            }
            else // sizes are equal
            {
                // nothing to do if the sizes are equal
            }
        }
        else if (castType->IsFloat())
        {
            Type* dstType = CreateLlvmType(castType);
            TypeInfo::ESign exprSign = exprType->GetSign();
            Instruction::CastOps castOp = (exprSign == TypeInfo::eSigned) ? Instruction::CastOps::SIToFP : Instruction::CastOps::UIToFP;
            resultValue = builder.CreateCast(castOp, resultValue, dstType, "cast");
        }
        else if (castType->IsBool())
        {
            Type* resultValueType = resultValue->getType();
            bool isSigned = exprType->GetSign() == TypeInfo::eSigned;
            Value* zero = ConstantInt::get(resultValueType, 0, isSigned);
            resultValue = builder.CreateICmpNE(resultValue, zero, "cast");
        }
        else
        {
            assert(false && "Invalid cast");
        }
    }
    else if (exprType->IsFloat())
    {
        if (castType->IsInt())
        {
            Type* dstType = CreateLlvmType(castType);
            TypeInfo::ESign exprSign = castType->GetSign();
            Instruction::CastOps castOp = (exprSign == TypeInfo::eSigned) ? Instruction::CastOps::FPToSI : Instruction::CastOps::FPToUI;
            resultValue = builder.CreateCast(castOp, resultValue, dstType, "cast");
        }
        else if (castType->IsBool())
        {
            APFloat::Semantics semantics = APFloat::S_IEEEdouble;
            switch (exprType->GetNumBits())
            {
                case 32:
                    semantics = APFloat::S_IEEEsingle;
                    break;
                case 64:
                    semantics = APFloat::S_IEEEdouble;
                    break;
                default:
                    assert(false && "Invalid float size in cast");
                    break;
            }

            Value* zero = ConstantFP::get(context, APFloat::getZero(APFloat::EnumToSemantics(semantics)));
            resultValue = builder.CreateFCmpONE(resultValue, zero, "cast");
        }
        else if (castType->IsFloat())
        {
            unsigned exprSize = exprType->GetNumBits();
            unsigned castSize = castType->GetNumBits();
            if (castSize < exprSize)
            {
                Type* dstType = CreateLlvmType(castType);
                resultValue = builder.CreateFPTrunc(resultValue, dstType, "cast");
            }
            else if (castSize > exprSize)
            {
                Type* dstType = CreateLlvmType(castType);
                resultValue = builder.CreateFPExt(resultValue, dstType, "cast");
            }
            else // sizes are equal
            {
                // nothing to do if the sizes are equal
            }
        }
        else
        {
            assert(false && "Invalid cast");
        }
    }
    else
    {
        assert(false && "Invalid cast");
    }
}

void LlvmIrGenerator::Visit(ImplicitCastExpression* castExpression)
{
    Expression* subExpression = castExpression->subExpression;
    subExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    const TypeInfo* exprType = subExpression->GetType();
    const TypeInfo* castType = castExpression->GetType();

    if (exprType->IsInt())
    {
        if (castType->IsInt())
        {
            resultValue = CreateExt(resultValue, exprType, castType);
        }
        else
        {
            assert(false && "Invalid implicit cast");
        }
    }
    else if (exprType->IsFloat())
    {
        if (castType->IsFloat())
        {
            assert(exprType->GetNumBits() < castType->GetNumBits());

            Type* dstType = CreateLlvmType(castType);
            resultValue = builder.CreateFPExt(resultValue, dstType, "fpext");
        }
        else
        {
            assert(false && "Invalid implicit cast");
        }
    }
    else
    {
        assert(false && "Invalid implicit cast");
    }
}

void LlvmIrGenerator::Visit(FunctionCallExpression* functionCallExpression)
{
    functionCallExpression->functionExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* funValue = resultValue;

    vector<Expression*> argExpressions = functionCallExpression->arguments;
    vector<Value*> args;
    for (size_t i = 0; i < argExpressions.size(); ++i)
    {
        Expression* expr = argExpressions[i];
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        args.push_back(resultValue);
    }

    FunctionType* funType = CreateLlvmFunctionType(functionCallExpression->functionType);

    FunctionCallee func(funType, funValue);
    resultValue = builder.CreateCall(func, args, "call");
}

void LlvmIrGenerator::Visit(MemberExpression* memberExpression)
{
    Expression* expr = memberExpression->subExpression;
    expr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    SetDebugLocation(memberExpression->memberNameToken);

    const TypeInfo* type = expr->GetType();
    bool isPointer = type->IsPointer();
    if (isPointer)
    {
        type = type->GetInnerType();
    }

    ROString memberName = memberExpression->memberName;
    const MemberInfo* member = type->GetMember(memberName);
    if (member == nullptr)
    {
        logger.LogInternalError("Member is null");
        resultValue = nullptr;
        return;
    }
    unsigned memberIndex = member->GetIndex();

    Expression::EAccessType accessType = memberExpression->GetAccessType();

    if (accessType == Expression::eAddress || isPointer)
    {
        Type* llvmStructType = CreateLlvmType(type);
        Type* llvmMemberType = CreateLlvmType(member->GetType());

        if (accessType == Expression::eAddress && isPointer)
        {
            Type* llvmStructPointerType = CreateLlvmType(expr->GetType());
            resultValue = builder.CreateLoad(llvmStructPointerType, resultValue, "load");
        }

        vector<Value*> indices;
        indices.push_back(ConstantInt::get(context, APInt(uIntSizeType->GetNumBits(), 0)));
        indices.push_back(ConstantInt::get(context, APInt(32, memberIndex)));

        // calculate member address
        Value* memberPointer = builder.CreateInBoundsGEP(llvmStructType, resultValue, indices, "mber");

        if (accessType == Expression::eValue)
        {
            // load member
            resultValue = builder.CreateLoad(llvmMemberType, memberPointer, "load");
        }
        else
        {
            resultValue = memberPointer;
        }
    }
    else
    {
        vector<unsigned> indices;
        indices.push_back(memberIndex);

        resultValue = builder.CreateExtractValue(resultValue, indices, "mber");
    }
}

void LlvmIrGenerator::Visit(BranchExpression* branchExpression)
{
    const TypeInfo* resultType = branchExpression->GetType();

    // generate the condition IR
    branchExpression->ifCondition->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* conditionValue = resultValue;

    // create the branch basic blocks
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* trueBlock = BasicBlock::Create(context, "if", function);
    BasicBlock* falseBlock = BasicBlock::Create(context, "else");
    BasicBlock* mergeBlock = BasicBlock::Create(context, "merge");

    builder.CreateCondBr(conditionValue, trueBlock, falseBlock);

    // generate "true" block IR
    builder.SetInsertPoint(trueBlock);

    Expression* ifExpr = branchExpression->ifExpression;
    ifExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    Value* ifExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "true" block
    trueBlock = builder.GetInsertBlock();

    // generate "false" block IR
    function->insert(function->end(), falseBlock);
    builder.SetInsertPoint(falseBlock);

    Expression* elseExpr = branchExpression->elseExpression;
    elseExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    Value* elseExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "false" block
    falseBlock = builder.GetInsertBlock();

    // generate merge block IR
    function->insert(function->end(), mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    Type* phiType = CreateLlvmType(resultType);
    PHINode* phiNode = builder.CreatePHI(phiType, 2, "phi");
    phiNode->addIncoming(ifExprValue, trueBlock);
    phiNode->addIncoming(elseExprValue, falseBlock);

    resultValue = phiNode;
}

void LlvmIrGenerator::Visit(ConstantDeclaration* constantDeclaration)
{
    ROString constName = constantDeclaration->name;
    const TypeInfo* constType = constantDeclaration->constantType;

    unsigned constIdx = constantDeclaration->assignmentExpression->right->GetConstantValueIndex();
    symbolTable.AddConstant(constName, constantDeclaration->nameToken, constType, constIdx);

    StructDefinitionExpression* structDef = dynamic_cast<StructDefinitionExpression*>(constantDeclaration->assignmentExpression->right);
    if (structDef != nullptr)
    {
        structDef->Accept(this);
    }

    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(VariableDeclaration* variableDeclaration)
{
    ROString varName = variableDeclaration->name;
    const TypeInfo* varType = variableDeclaration->variableType;
    Type* type = CreateLlvmType(varType);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown variable declaration type");
        return;
    }

    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, variableDeclaration->nameToken, varType, alloca);
    CreateDebugVariable(variableDeclaration->nameToken, varType, alloca);

    variableDeclaration->assignmentExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    // variable declaration expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

bool LlvmIrGenerator::Generate(Modules* syntaxTree, Module*& module)
{
    unsigned firstFileId = syntaxTree->modules.front()->fileId;
    const string& firstInFilename = compilerContext.GetFilename(firstFileId);

    module = new Module(firstInFilename, context);
    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetMachine->getTargetTriple().str());

    if (dbgInfo)
    {
        // initialize debug info builder
        diBuilder = new DIBuilder(*module);

        // create file debug info
        diFiles = new DIFile*[syntaxTree->modules.size()];
        for (ModuleDefinition* moduleDefinition : syntaxTree->modules)
        {
            unsigned fileId = moduleDefinition->fileId;
            const string& filename = compilerContext.GetFilename(fileId);

            diFiles[fileId] = diBuilder->createFile(filename, fs::current_path().string());
        }
    }

    ArrayRef<Type*> emptyArray;
    unitType = StructType::create(context, emptyArray, "UnitType");

    strStructElements[0] = CreateLlvmType(uIntSizeType);
    strStructElements[1] = PointerType::get(Type::getInt8Ty(context), 0);

    ArrayRef<Type*> strArrayRef(strStructElements, STR_STRUCT_ELEMENTS_SIZE);
    strStructType = StructType::create(context, strArrayRef, "str");

    // register types
    types.insert({TypeInfo::BoolType->GetId(), boolType});
    types.insert({compilerContext.typeRegistry.GetStringType()->GetId(), strStructType});

    // create LLVM types
    for (const TypeInfo* type : compilerContext.typeRegistry)
    {
        CreateLlvmType(type);

        if (dbgInfo)
        {
            CreateLlvmDebugType(type);
        }
    }

    // add struct members
    vector<Type*> structMembers;
    for (const TypeInfo* type : compilerContext.typeRegistry)
    {
        if (type->IsAggregate())
        {
            structMembers.clear();
            for (const MemberInfo* member : type->GetMembers())
            {
                Type* llvmMemberType = CreateLlvmType(member->GetType());
                assert(llvmMemberType != nullptr && "Unkown member definition type");
                structMembers.push_back(llvmMemberType);
            }

            auto iter = types.find(type->GetId());
            assert(iter != types.end() && "Could not find struct LLVM type");
            StructType* structType = static_cast<StructType*>(iter->second);
            structType->setBody(structMembers);

            // add debug info for struct members
            if (dbgInfo)
            {
                if (type->GetToken() != nullptr)
                {
                    DIFile* diFile = diFiles[type->GetToken()->filenameId];

                    SmallVector<Metadata*, 8> elements;

                    uint64_t offset = 0;
                    for (const MemberInfo* member : type->GetMembers())
                    {
                        const TypeInfo* memberType = member->GetType();
                        DIType* memberDiType = CreateLlvmDebugType(memberType);
                        assert(memberDiType != nullptr && "Unknown debug member definition type");

                        uint64_t memberSize = memberDiType->getSizeInBits();
                        unsigned pointerSize = compilerContext.typeRegistry.GetPointerSize();
                        uint32_t alignment = (memberSize > pointerSize) ? pointerSize : static_cast<uint32_t>(memberSize);
                        if (alignment > 0)
                        {
                            uint32_t rem = offset % alignment;
                            if (rem > 0)
                            {
                                offset += alignment - rem;
                            }
                        }

                        ROString memberName = member->GetName();
                        unsigned memberLine = member->GetToken()->line;
                        elements.push_back(diBuilder->createMemberType(diFile, toStringRef(memberName), diFile, memberLine, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

                        offset += memberSize;
                    }

                    DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
                    auto iter = diTypes.find(type->GetId());
                    assert(iter != diTypes.end());
                    DICompositeType* diType = static_cast<DICompositeType*>(iter->second);
                    diType->replaceElements(elementsArray);
                }
            }
        }
    }

    // generate LLVM IR from syntax tree
    this->module = module;
    syntaxTree->Accept(this);

    if (resultValue == nullptr)
    {
        delete module;
        module = nullptr;
        return false;
    }

    if (dbgInfo)
    {
        diBuilder->finalize();

        delete [] diFiles;
    }

    return true;
}

FunctionType* LlvmIrGenerator::CreateLlvmFunctionType(const TypeInfo* type)
{
    // get the return type
    Type* returnType = CreateLlvmType(type->GetReturnType());
    if (returnType == nullptr)
    {
        logger.LogInternalError("Invalid function return type");
        return nullptr;
    }

    // get the parameter types
    vector<Type*> paramTypes;
    for (const TypeInfo* paramTypeInfo : type->GetParamTypes())
    {
        Type* paramType = CreateLlvmType(paramTypeInfo);
        if (paramType == nullptr)
        {
            logger.LogInternalError("Invalid function param type");
            return nullptr;
        }
        paramTypes.push_back(paramType);
    }

    FunctionType* funType = FunctionType::get(returnType, paramTypes, false);
    return funType;
}

Type* LlvmIrGenerator::CreateLlvmType(const TypeInfo* type)
{
    // try to lookup this type to see if it's already been created
    auto iter = types.find(type->GetId());
    if (iter != types.end())
    {
        return iter->second;
    }

    Type* llvmType = nullptr;
    if (type->IsSameAs(*TypeInfo::UnitType))
    {
        llvmType = unitType;
    }
    else if (type->IsInt())
    {
        unsigned numBits = type->GetNumBits();
        llvmType = Type::getIntNTy(context, numBits);
    }
    else if (type->IsFloat())
    {
        unsigned numBits = type->GetNumBits();
        if (numBits == 32)
        {
            llvmType = Type::getFloatTy(context);
        }
        else if (numBits == 64)
        {
            llvmType = Type::getDoubleTy(context);
        }
    }
    else if (type->IsPointer())
    {
        Type* innerType = CreateLlvmType(type->GetInnerType());
        if (innerType != nullptr)
        {
            llvmType = innerType->getPointerTo();
        }
    }
    else
    {
        // if this is a range type, create the LLVM type
        if (type->IsRange())
        {
            vector<Type*> members;
            for (const MemberInfo* memberInfo : type->GetMembers())
            {
                Type* memberType = CreateLlvmType(memberInfo->GetType());
                if (memberType == nullptr)
                {
                    return nullptr;
                }

                members.push_back(memberType);
            }

            llvmType = StructType::create(context, members, toStringRef(type->GetShortName()));
        }
        else if (type->IsArray())
        {
            Type* innerType = CreateLlvmType(type->GetInnerType());
            if (innerType != nullptr)
            {
                Type* arrayStructElements[2];
                arrayStructElements[0] = CreateLlvmType(uIntSizeType);
                arrayStructElements[1] = PointerType::get(innerType, 0);

                ArrayRef<Type*> arrayRef(arrayStructElements, 2);
                llvmType = StructType::create(context, arrayRef, toStringRef(type->GetShortName()));
            }
        }
        else if (type->IsFunction())
        {
            FunctionType* funType = CreateLlvmFunctionType(type);
            if (funType == nullptr)
            {
                return nullptr;
            }

            llvmType = funType->getPointerTo();
        }
        else if (type->IsAggregate())
        {
            llvmType = StructType::create(context, toStringRef(type->GetShortName()));
        }
        else // could not determine the type
        {
            llvmType = nullptr;
        }
    }

    if (llvmType != nullptr)
    {
        // register type so we don't have to create it again
        types.insert({type->GetId(), llvmType});
    }

    return llvmType;
}

DIType* LlvmIrGenerator::CreateLlvmDebugType(const TypeInfo* type)
{
    // try to lookup this type to see if it's already been created
    auto iter = diTypes.find(type->GetId());
    if (iter != diTypes.end())
    {
        return iter->second;
    }

    DIType* diType = nullptr;
    if (type->IsInt())
    {
        ROString name = type->GetShortName();
        unsigned numBits = type->GetNumBits();
        unsigned encoding = (type->GetSign() == TypeInfo::eSigned) ? dwarf::DW_ATE_signed : dwarf::DW_ATE_unsigned;
        diType = diBuilder->createBasicType(toStringRef(name), numBits, encoding);
    }
    else if (type->IsFloat())
    {
        ROString name = type->GetShortName();
        unsigned numBits = type->GetNumBits();
        unsigned encoding = dwarf::DW_ATE_float;
        diType = diBuilder->createBasicType(toStringRef(name), numBits, encoding);
    }
    else if (type->IsBool())
    {
        ROString name = type->GetShortName();
        diType = diBuilder->createBasicType(toStringRef(name), 8, dwarf::DW_ATE_boolean);
    }
    else if (type->IsPointer())
    {
        DIType* innerDiType = CreateLlvmDebugType(type->GetInnerType());
        if (innerDiType == nullptr)
        {
            return nullptr;
        }
        diType = diBuilder->createPointerType(innerDiType, compilerContext.typeRegistry.GetPointerSize(), 0, {}, toStringRef(type->GetShortName()));
    }
    else if (type->IsSameAs(*TypeInfo::UnitType))
    {
        diType = diBuilder->createBasicType(toStringRef(TypeInfo::UnitType->GetShortName()), 0, dwarf::DW_ATE_unsigned);
    }
    else if (type->IsStr())
    {
        const TypeInfo* strType = compilerContext.typeRegistry.GetStringType();
        ROString name = strType->GetShortName();
        unsigned numBits = strType->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : strType->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = CreateLlvmDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            ROString memberName = member->GetName();
            unsigned size = memberType->GetNumBits();
            // TODO: don't hard-code alignment
            elements.push_back(diBuilder->createMemberType(nullptr, toStringRef(memberName), nullptr, 0, size, 4, offset, DINode::FlagZero, memberDiType));

            offset += size;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        diType = diBuilder->createStructType(nullptr, toStringRef(name), nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else if (type->IsArray())
    {
        ROString name = type->GetShortName();
        unsigned numBits = type->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : type->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = CreateLlvmDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            ROString memberName = member->GetName();
            unsigned size = memberType->GetNumBits();
            // TODO: don't hard-code alignment
            elements.push_back(diBuilder->createMemberType(nullptr, toStringRef(memberName), nullptr, 0, size, 4, offset, DINode::FlagZero, memberDiType));

            offset += size;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        diType = diBuilder->createStructType(nullptr, toStringRef(name), nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else if (type->IsRange())
    {
        ROString name = type->GetShortName();
        unsigned numBits = type->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : type->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = CreateLlvmDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            ROString memberName = member->GetName();
            uint64_t memberSize = memberDiType->getSizeInBits();
            // TODO: better way to get alignment?
            uint32_t alignment = (memberSize > 32) ? 32 : static_cast<uint32_t>(memberSize);
            elements.push_back(diBuilder->createMemberType(nullptr, toStringRef(memberName), nullptr, 0, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

            offset += memberSize;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);

        // TODO: set alignment
        diType = diBuilder->createStructType(nullptr, toStringRef(name), nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else if (type->IsFunction())
    {
        SmallVector<Metadata*, 8> funTypes;
        DIType* retDebugType = CreateLlvmDebugType(type->GetReturnType());
        if (retDebugType == nullptr)
        {
            return nullptr;
        }
        funTypes.push_back(retDebugType);

        for (const TypeInfo* paramType : type->GetParamTypes())
        {
            DIType* paramDebugType = CreateLlvmDebugType(paramType);
            if (paramDebugType == nullptr)
            {
                return nullptr;
            }
            funTypes.push_back(paramDebugType);
        }

        DISubroutineType* subroutine = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(funTypes));
        diType = diBuilder->createPointerType(subroutine, compilerContext.typeRegistry.GetPointerSize(), 0, {}, toStringRef(type->GetShortName()));
    }
    else if (type->IsAggregate())
    {
        DIFile* diFile = nullptr;
        unsigned line = 0;

        const Token* token = type->GetToken();
        if (token->type != Token::eInvalid)
        {
            diFile = diFiles[token->filenameId];
            line = token->line;
        }

        ROString name = type->GetShortName();
        unsigned numBits = type->GetNumBits();

        // TODO: set alignment
        SmallVector<Metadata*, 0> elements;
        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        diType = diBuilder->createStructType(diFile, toStringRef(name), diFile, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else
    {
        // TODO: delete this?
        ROString name = type->GetShortName();
        auto iter = diStructTypes.find(name);
        if (iter != diStructTypes.end())
        {
            diType = iter->second;
        }
        else
        {
            logger.LogInternalError("Could not determine debug type");
        }
    }

    if (diType != nullptr)
    {
        // register type so we don't have to create it again
        diTypes.insert({type->GetId(), diType});
    }

    return diType;
}

bool LlvmIrGenerator::CreateFunctionDeclaration(const FunctionDeclaration* funcDecl)
{
    // get the return type
    Type* returnType = CreateLlvmType(funcDecl->returnType);
    if (returnType == nullptr)
    {
        logger.LogInternalError("Invalid function return type");
        return false;
    }

    // get the parameter types
    const Parameters& declParams = funcDecl->parameters;
    vector<Type*> parameters;
    parameters.reserve(declParams.size());
    for (const Parameter* declParam : declParams)
    {
        Type* varType = CreateLlvmType(declParam->type);
        parameters.push_back(varType);
    }

    FunctionType* funcType = FunctionType::get(returnType, parameters, false);
    Function::Create(funcType, Function::ExternalLinkage, toStringRef(funcDecl->name), module);

    return true;
}

AllocaInst* LlvmIrGenerator::CreateVariableAlloc(Function* function, Type* type, ROString paramName)
{
    // create builder to insert alloca at beginning of function
    BasicBlock& entryBlock = function->getEntryBlock();
    IRBuilder<> tempBuilder(&entryBlock, entryBlock.begin());

    AllocaInst* alloca = tempBuilder.CreateAlloca(type, nullptr, toStringRef(paramName));
    return alloca;
}

Value* LlvmIrGenerator::CreateExt(llvm::Value* value, const TypeInfo* valueType, const TypeInfo* dstType)
{
    Type* llvmType = CreateLlvmType(dstType);

    if (valueType->GetSign() == TypeInfo::eContextDependent)
    {
        value = builder.CreateZExt(value, llvmType, "zeroext");
    }
    else
    {
        if (dstType->GetSign() == TypeInfo::eSigned)
        {
            value = builder.CreateSExt(value, llvmType, "signext");
        }
        else
        {
            value = builder.CreateZExt(value, llvmType, "zeroext");
        }
    }

    return value;
}

Value* LlvmIrGenerator::CreateLogicalBranch(Expression* conditionExpr, Expression* branchExpr, bool isAnd)
{
    const char* branchName = isAnd ? "andtrue" : "orfalse";
    const char* mergeName = isAnd ? "andmerge" : "ormerge";
    const char* phiName = isAnd ? "andphi" : "orphi";

    // generate the condition IR
    conditionExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }
    Value* conditionValue = resultValue;

    BasicBlock* startBlock = builder.GetInsertBlock();

    // create the branch basic blocks
    Function* function = startBlock->getParent();
    BasicBlock* branchBlock = BasicBlock::Create(context, branchName, function);
    BasicBlock* mergeBlock = BasicBlock::Create(context, mergeName);

    if (isAnd)
    {
        builder.CreateCondBr(conditionValue, branchBlock, mergeBlock);
    }
    else
    {
        builder.CreateCondBr(conditionValue, mergeBlock, branchBlock);
    }

    // generate branch block IR
    builder.SetInsertPoint(branchBlock);

    branchExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }

    Value* branchExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the branch block
    branchBlock = builder.GetInsertBlock();

    // generate merge block IR
    function->insert(function->end(), mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    PHINode* phiNode = builder.CreatePHI(boolType, 2, phiName);
    phiNode->addIncoming(branchExprValue, branchBlock);
    phiNode->addIncoming(isAnd ? ConstantInt::getFalse(context) : ConstantInt::getTrue(context), startBlock);

    return phiNode;
}

void LlvmIrGenerator::SetDebugLocation(const Token* token)
{
    if (dbgInfo)
    {
        unsigned line = token->line;
        unsigned column = token->column;
        builder.SetCurrentDebugLocation(DILocation::get(context, line, column, diScopes.top()));
    }
}

void LlvmIrGenerator::CreateDebugVariable(const Token* token, const TypeInfo* type, AllocaInst* alloca)
{
    if (dbgInfo)
    {
        unsigned line = token->line;
        unsigned column = token->column;
        DIType* varDebugType = CreateLlvmDebugType(type);
        if (varDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        ROString value = token->value;
        DIScope* diScope = diScopes.top();
        DILocalVariable* diVar = diBuilder->createAutoVariable(diScope, toStringRef(value), currentDiFile, line, varDebugType, true);
        diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DILocation::get(context, line, column, diScope), builder.GetInsertBlock());
    }
}
