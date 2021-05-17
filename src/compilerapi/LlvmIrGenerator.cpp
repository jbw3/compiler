#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4141 4146 4244 4267 4624 6001 6011 6297 26439 26450 26451 26495 26812)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
#include "LlvmIrGenerator.h"
#include "Config.h"
#include "ErrorLogger.h"
#include "SyntaxTree.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Target/TargetMachine.h"
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma clang diagnostic pop
#endif

namespace fs = std::experimental::filesystem;
using namespace llvm;
using namespace std;
using namespace SyntaxTree;

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

LlvmIrGenerator::LlvmIrGenerator(CompilerContext& compilerContext, const Config& config, ErrorLogger& logger) :
    targetMachine(config.targetMachine),
    optimizationLevel(config.optimizationLevel),
    dbgInfo(config.debugInfo),
    boundsCheck(config.boundsCheck),
    compilerContext(compilerContext),
    logger(logger),
    builder(context),
    diBuilder(nullptr),
    module(nullptr),
    currentFunction(nullptr),
    resultValue(nullptr),
    unitType(nullptr),
    strStructType(nullptr),
    globalStringCounter(0),
    boolType(Type::getInt1Ty(context))
{
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
            resultValue = builder.CreateNeg(subExprValue, "neg");
            break;
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
                resultValue = builder.CreateLoad(subExprValue, "load");
            }
            break;
        }
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
                leftValue = builder.CreateLoad(leftValue, "load");
            }
        }

        bool isSigned = leftType->GetSign() == TypeInfo::eSigned;

        switch (op)
        {
            case BinaryExpression::eEqual:
                resultValue = builder.CreateICmpEQ(leftValue, rightValue, "cmpeq");
                break;
            case BinaryExpression::eNotEqual:
                resultValue = builder.CreateICmpNE(leftValue, rightValue, "cmpne");
                break;
            case BinaryExpression::eLessThan:
                resultValue = isSigned ? builder.CreateICmpSLT(leftValue, rightValue, "cmplt") : builder.CreateICmpULT(leftValue, rightValue, "cmplt");
                break;
            case BinaryExpression::eLessThanOrEqual:
                resultValue = isSigned ? builder.CreateICmpSLE(leftValue, rightValue, "cmple") : builder.CreateICmpULE(leftValue, rightValue, "cmple");
                break;
            case BinaryExpression::eGreaterThan:
                resultValue = isSigned ? builder.CreateICmpSGT(leftValue, rightValue, "cmpgt") : builder.CreateICmpUGT(leftValue, rightValue, "cmpgt");
                break;
            case BinaryExpression::eGreaterThanOrEqual:
                resultValue = isSigned ? builder.CreateICmpSGE(leftValue, rightValue, "cmpge") : builder.CreateICmpUGE(leftValue, rightValue, "cmpge");
                break;
            case BinaryExpression::eAdd:
            case BinaryExpression::eAddAssign:
                resultValue = builder.CreateAdd(leftValue, rightValue, "add");
                break;
            case BinaryExpression::eSubtract:
            case BinaryExpression::eSubtractAssign:
                resultValue = builder.CreateSub(leftValue, rightValue, "sub");
                break;
            case BinaryExpression::eMultiply:
            case BinaryExpression::eMultiplyAssign:
                resultValue = builder.CreateMul(leftValue, rightValue, "mul");
                break;
            case BinaryExpression::eDivide:
            case BinaryExpression::eDivideAssign:
                resultValue = isSigned ? builder.CreateSDiv(leftValue, rightValue, "div") : builder.CreateUDiv(leftValue, rightValue, "div");
                break;
            case BinaryExpression::eRemainder:
            case BinaryExpression::eRemainderAssign:
                resultValue = isSigned ? builder.CreateSRem(leftValue, rightValue, "rem") : builder.CreateURem(leftValue, rightValue, "rem");
                break;
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
                Type* rangeType = GetType(binaryExpression->GetType());
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

    unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();

    if (boundsCheck)
    {
        vector<uint32_t> sizeIndex;
        sizeIndex.push_back(0);
        Value* size = builder.CreateExtractValue(leftValue, sizeIndex, "size");

        Value* indexValue = nullptr;
        if (rightType->GetNumBits() < uIntSizeNumBits)
        {
            Type* extType = GetType(TypeInfo::GetUIntSizeType());
            indexValue = builder.CreateZExt(rightValue, extType, "zeroext");
        }
        else
        {
            indexValue = rightValue;
        }

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

            const string& filename = compilerContext.GetFilename(opToken->filenameId);
            Constant* fileStrPtr = CreateConstantString(filename);
            Value* fileStr = builder.CreateLoad(fileStrPtr, "filestr");
            uint64_t line = static_cast<uint64_t>(opToken->line);
            Constant* lineNum = ConstantInt::get(context, APInt(32, line, false));
            Constant* msgStrPtr = CreateConstantString("Index is out of bounds");
            Value* msgStr = builder.CreateLoad(msgStrPtr, "msgstr");

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
    valueIndices.push_back(rightValue);
    Value* valuePtr = builder.CreateInBoundsGEP(data, valueIndices, "value");
    Value* result = nullptr;
    if (binaryExpression->GetAccessType() == Expression::eAddress)
    {
        result = valuePtr;
    }
    else
    {
        result = builder.CreateLoad(valuePtr, "load");
    }

    return result;
}

Value* LlvmIrGenerator::GenerateRangeSubscriptIr(const BinaryExpression* binaryExpression, Value* leftValue, Value* rightValue)
{
    unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();
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
        Type* extType = GetType(TypeInfo::GetUIntSizeType());
        start = builder.CreateZExt(start, extType, "zeroext");
    }

    // get range end
    Value* end = builder.CreateExtractValue(rightValue, 1, "end");
    if (innerTypeNumBits < uIntSizeNumBits)
    {
        Type* extType = GetType(TypeInfo::GetUIntSizeType());
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

    vector<Value*> indices;
    indices.push_back(checkStart);
    Value* newData = builder.CreateInBoundsGEP(data, indices, "ptr");

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
    const string& varName = forLoop->variableName;
    const TypeInfo* varType = forLoop->variableType;
    bool isSigned = varType->GetSign() == TypeInfo::eSigned;
    Type* type = GetType(varType);
    assert(type != nullptr && "Unknown variable declaration type");
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, varType, alloca);
    CreateDebugVariable(varNameToken, varType, alloca);

    // create index
    const string& indexName = forLoop->indexName;
    bool hasIndex = !indexName.empty();
    const TypeInfo* indexTypeInfo = forLoop->indexType;
    Type* indexType = nullptr;
    AllocaInst* indexAlloca = nullptr;
    if (hasIndex)
    {
        indexType = GetType(indexTypeInfo);
        assert(indexType != nullptr && "Unknown variable declaration type");
        indexAlloca = CreateVariableAlloc(currentFunction, indexType, indexName);
        symbolTable.AddVariable(indexName, indexTypeInfo, indexAlloca);
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
        iterType = GetType(TypeInfo::GetUIntSizeType());

        vector<unsigned> index(1);
        unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();

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
        Value* valuePtr = builder.CreateInBoundsGEP(data, valueIndices, "value");
        Value* value = builder.CreateLoad(valuePtr, "load");
        value = CreateExt(value, iterableInnerType, varType);
        builder.CreateStore(value, alloca);

        if (hasIndex)
        {
            SetDebugLocation(forLoop->indexNameToken);

            // zero extend if necessary
            Value* store = nullptr;
            if (indexTypeInfo->GetNumBits() > TypeInfo::GetUIntSizeType()->GetNumBits())
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

void LlvmIrGenerator::Visit(ExternFunctionDeclaration* /*externFunctionDeclaration*/)
{
    // nothing to do here
}

void LlvmIrGenerator::Visit(FunctionDefinition* functionDefinition)
{
    const FunctionDeclaration* declaration = functionDefinition->declaration;
    const string& funcName = declaration->name;
    Function* func = module->getFunction(funcName);
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
        DIType* retDebugType = GetDebugType(declaration->returnType);
        if (retDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        funTypes.push_back(retDebugType);

        for (const Parameter* param : params)
        {
            DIType* paramDebugType = GetDebugType(param->type);
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            funTypes.push_back(paramDebugType);
        }

        DISubroutineType* subroutine = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(funTypes), DINode::FlagPrototyped);

        unsigned line = declaration->nameToken->line;
        diSubprogram = diBuilder->createFunction(currentDiFile, funcName, "", currentDiFile, line, subroutine, 0, DINode::FlagZero, DISubprogram::SPFlagDefinition);
        func->setSubprogram(diSubprogram);

        // unset debug location for function prolog
        builder.SetCurrentDebugLocation(DebugLoc());
    }

    DebugScope dbgScope(dbgInfo, diScopes, diSubprogram);

    size_t idx = 0;
    for (Argument& arg : func->args())
    {
        const Parameter* param = params[idx];
        const string& paramName = param->name;
        arg.setName(paramName);
        AllocaInst* alloca = CreateVariableAlloc(func, arg.getType(), paramName);
        builder.CreateStore(&arg, alloca);
        const TypeInfo* paramType = param->type;
        symbolTable.AddVariable(paramName, paramType, alloca);

        if (dbgInfo)
        {
            const Token* token = param->nameToken;
            unsigned line = token->line;
            DIType* paramDebugType = GetDebugType(paramType);
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            DILocalVariable* diVar = diBuilder->createParameterVariable(diSubprogram, paramName, idx + 1, currentDiFile, line, paramDebugType, true);
            diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DebugLoc::get(line, 0, diSubprogram), builder.GetInsertBlock());
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
    const string& structName = structDefinition->name;
    const TypeInfo* typeInfo = structDefinition->type;

    vector<Type*> members;
    for (const MemberDefinition* memberDef : structDefinition->members)
    {
        const MemberInfo* memberInfo = typeInfo->GetMember(memberDef->name);
        Type* memberType = GetType(memberInfo->GetType());
        if (memberType == nullptr)
        {
            resultValue = nullptr;
            logger.LogInternalError("Unknown member definition type");
            return;
        }

        members.push_back(memberType);
    }

    auto iter = types.find(structName);
    assert(iter != types.end());
    StructType* structType = static_cast<StructType*>(iter->second);
    structType->setBody(members);

    if (dbgInfo)
    {
        const AggregateType* aggType = dynamic_cast<const AggregateType*>(typeInfo);

        DIFile* file = diFiles[structDefinition->fileId];

        SmallVector<Metadata*, 8> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : aggType->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = GetDebugType(memberType);
            if (memberDiType == nullptr)
            {
                resultValue = nullptr;
                return;
            }

            const string& memberName = member->GetName();
            uint64_t memberSize = memberDiType->getSizeInBits();
            // TODO: better way to get alignment?
            uint64_t alignment = (memberSize > 32) ? 32 : memberSize;
            unsigned memberLine = member->GetToken()->line;
            elements.push_back(diBuilder->createMemberType(file, memberName, file, memberLine, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

            offset += memberSize;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        auto iter = diStructTypes.find(structName);
        assert(iter != diStructTypes.end());
        DICompositeType* diType = iter->second;
        diType->replaceElements(elementsArray);
    }
}

void LlvmIrGenerator::Visit(StructInitializationExpression* structInitializationExpression)
{
    const TypeInfo* typeInfo = structInitializationExpression->GetType();
    Type* type = GetType(typeInfo);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown member definition type");
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
    // create file debug info
    if (dbgInfo)
    {
        for (ModuleDefinition* moduleDefinition : modules->modules)
        {
            unsigned fileId = moduleDefinition->fileId;
            const string& filename = compilerContext.GetFilename(fileId);

            diFiles[fileId] = diBuilder->createFile(filename, fs::current_path().string());
        }
    }

    // add all struct names to types map
    for (StructDefinition* structDef : modules->orderedStructDefinitions)
    {
        const string& structName = structDef->name;
        StructType* structType = StructType::create(context, structName);
        types.insert({structName, structType});

        // add debug info for structs
        if (dbgInfo)
        {
            DIFile* diFile = diFiles[structDef->fileId];

            const AggregateType* aggType = dynamic_cast<const AggregateType*>(structDef->type);

            const string& name = aggType->GetShortName();
            unsigned numBits = aggType->GetNumBits();

            unsigned line = aggType->GetToken()->line;
            // TODO: set alignment
            SmallVector<Metadata*, 0> elements;
            DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
            DICompositeType* diType = diBuilder->createStructType(diFile, name, diFile, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
            diStructTypes.insert({structDef->name, diType});
        }
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
    resultValue = builder.CreateLoad(strPtr, "load");
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
        const TypeInfo* sizeType = TypeInfo::GetUIntSizeType();

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
            ConstantInt::get(context, APInt(sizeType->GetNumBits(), numChars, false)),
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

void LlvmIrGenerator::Visit(IdentifierExpression* identifierExpression)
{
    const string& name = identifierExpression->name;
    const SymbolTable::VariableData* data = symbolTable.GetVariableData(name);
    if (data == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("No alloca found for '{}'", name);
        return;
    }

    SetDebugLocation(identifierExpression->token);

    if (data->IsConstant())
    {
        unsigned constIdx = data->constValueIndex;
        const ConstantValue& value = compilerContext.GetConstantValue(constIdx);

        const TypeInfo* type = identifierExpression->GetType();
        if (type->IsBool())
        {
            resultValue = value.boolValue ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
        }
        else if (type->IsInt())
        {
            unsigned numBits = type->GetNumBits();
            bool isSigned = type->GetSign() == TypeInfo::eSigned;
            resultValue = ConstantInt::get(context, APInt(numBits, value.intValue, isSigned));
        }
        else
        {
            resultValue = nullptr;
            logger.LogInternalError("Unexpected type for constant '{}'", name);
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
                resultValue = builder.CreateLoad(alloca, name);
                break;
            case Expression::eAddress:
                resultValue = alloca;
                break;
        }
    }
}

void LlvmIrGenerator::Visit(ArraySizeValueExpression* arrayExpression)
{
    unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();

    Expression* sizeExpression = arrayExpression->sizeExpression;

    sizeExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* sizeValue = resultValue;
    if (sizeExpression->GetType()->GetNumBits() < uIntSizeNumBits)
    {
        Type* extType = GetType(TypeInfo::GetUIntSizeType());
        sizeValue = builder.CreateZExt(sizeValue, extType, "zeroext");
    }
    unsigned constIdx = sizeExpression->GetConstantValueIndex();
    const ConstantValue& constValue = compilerContext.GetConstantValue(constIdx);
    uint64_t arraySize = static_cast<uint64_t>(constValue.intValue);

    const TypeInfo* typeInfo = arrayExpression->GetType();
    const TypeInfo* innerTypeInfo = typeInfo->GetInnerType();
    Type* arrayType = GetType(typeInfo);
    Type* innerType = GetType(innerTypeInfo);
    Type* llvmArrayType = ArrayType::get(innerType, arraySize);
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, llvmArrayType, "array");

    Expression* valueExpression = arrayExpression->valueExpression;
    valueExpression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* valueValue = resultValue;

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
        Value* startPtr = builder.CreateInBoundsGEP(alloca, indices, "startPtr");

        // get address of end (1 past the last element)
        indices[1] = sizeValue;
        Value* endPtr = builder.CreateInBoundsGEP(alloca, indices, "endPtr");

        // branch to loop body block
        builder.CreateBr(loopBodyBlock);

        // set insert point to the loop body block
        builder.SetInsertPoint(loopBodyBlock);

        PHINode* phi = builder.CreatePHI(startPtr->getType(), 2, "phi");
        phi->addIncoming(startPtr, preLoopBlock);

        // store the value
        builder.CreateStore(valueValue, phi);

        // increment the address
        Value* nextPtr = builder.CreateInBoundsGEP(phi, one, "nextPtr");
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

    resultValue = structValue;
}

void LlvmIrGenerator::Visit(ArrayMultiValueExpression* arrayExpression)
{
    const Expressions& expressions = arrayExpression->expressions;
    uint64_t arraySize = expressions.size();

    const TypeInfo* typeInfo = arrayExpression->GetType();
    Type* arrayType = GetType(typeInfo);
    Type* innerType = GetType(typeInfo->GetInnerType());
    Type* llvmArrayType = ArrayType::get(innerType, arraySize);
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, llvmArrayType, "array");

    unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();
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

        Value* ptr = builder.CreateInBoundsGEP(alloca, indices, "ptr");
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
            Type* dstType = GetType(castType);
            resultValue = builder.CreateZExt(resultValue, dstType, "cast");
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
                Type* dstType = GetType(castType);
                resultValue = builder.CreateTrunc(resultValue, dstType, "cast");
            }
            else if (castSize > exprSize)
            {
                Type* dstType = GetType(castType);

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
    else
    {
        assert(false && "Invalid implicit cast");
    }
}

void LlvmIrGenerator::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->name;
    Function* func = module->getFunction(funcName);
    if (func == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Use of undeclared function '{}'", funcName);
        return;
    }

    if (functionExpression->arguments.size() != func->arg_size())
    {
        resultValue = nullptr;
        logger.LogInternalError("Unexpected number of function arguments");
        return;
    }

    const FunctionDeclaration* funcDecl = functionExpression->functionDeclaration;
    const Parameters& declParams = funcDecl->parameters;
    vector<Expression*> argExpressions = functionExpression->arguments;
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

    SetDebugLocation(functionExpression->nameToken);

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

    const string& memberName = memberExpression->memberName;
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
        if (accessType == Expression::eAddress && isPointer)
        {
            resultValue = builder.CreateLoad(resultValue, "load");
        }

        vector<Value*> indices;
        indices.push_back(ConstantInt::get(context, APInt(TypeInfo::GetUIntSizeType()->GetNumBits(), 0)));
        indices.push_back(ConstantInt::get(context, APInt(32, memberIndex)));

        // calculate member address
        Value* memberPointer = builder.CreateInBoundsGEP(resultValue, indices, "mber");

        if (accessType == Expression::eValue)
        {
            // load member
            resultValue = builder.CreateLoad(memberPointer, "load");
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
    function->getBasicBlockList().push_back(falseBlock);
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
    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    Type* phiType = GetType(resultType);
    PHINode* phiNode = builder.CreatePHI(phiType, 2, "phi");
    phiNode->addIncoming(ifExprValue, trueBlock);
    phiNode->addIncoming(elseExprValue, falseBlock);

    resultValue = phiNode;
}

void LlvmIrGenerator::Visit(ConstantDeclaration* constantDeclaration)
{
    const string& constName = constantDeclaration->name;
    const TypeInfo* constType = constantDeclaration->constantType;
    Type* type = GetType(constType);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown constant declaration type");
        return;
    }

    unsigned constIdx = constantDeclaration->assignmentExpression->right->GetConstantValueIndex();
    symbolTable.AddConstant(constName, constType, constIdx);

    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(VariableDeclaration* variableDeclaration)
{
    const string& varName = variableDeclaration->name;
    const TypeInfo* varType = variableDeclaration->variableType;
    Type* type = GetType(varType);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown variable declaration type");
        return;
    }

    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, varType, alloca);
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

    ArrayRef<Type*> emptyArray;
    unitType = StructType::create(context, emptyArray, "UnitType");

    unsigned int addressSpace = module->getDataLayout().getProgramAddressSpace();

    strStructElements[0] = GetType(TypeInfo::GetUIntSizeType());
    strStructElements[1] = PointerType::get(Type::getInt8Ty(context), 0);

    ArrayRef<Type*> strArrayRef(strStructElements, STR_STRUCT_ELEMENTS_SIZE);
    strStructType = StructType::create(context, strArrayRef, "str");

    // register types
    types.insert({TypeInfo::BoolType->GetShortName(), boolType});
    types.insert({TypeInfo::GetStringType()->GetShortName(), strStructType});

    if (dbgInfo)
    {
        // initialize debug info builder
        diBuilder = new DIBuilder(*module);

        diFiles = new DIFile*[syntaxTree->modules.size()];
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

string createTypeName(const TypeInfo* type)
{
    string name;
    string postfix;

    while (type->IsArray())
    {
        type = type->GetInnerType();
        name += "[";
        postfix += "]";
    }

    if (type->IsRange())
    {
        unsigned memberNumBits = type->GetInnerType()->GetNumBits();
        name += "Range" + to_string(memberNumBits);
    }
    else if (type->IsInt())
    {
        if (type->GetSign() == TypeInfo::eSigned)
        {
            name += "i";
        }
        else
        {
            name += "u";
        }
        name += to_string(type->GetNumBits());
    }
    else
    {
        name += type->GetShortName();
    }

    name += postfix;

    return name;
}

Type* LlvmIrGenerator::GetType(const TypeInfo* type)
{
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
    else if (type->IsPointer())
    {
        Type* innerType = GetType(type->GetInnerType());
        if (innerType != nullptr)
        {
            llvmType = innerType->getPointerTo();
        }
    }
    else
    {
        // get the LLVM type name
        string llvmName = createTypeName(type);

        // try to lookup this type to see if it's already been created
        auto iter = types.find(llvmName);
        if (iter != types.end())
        {
            llvmType = iter->second;
        }
        // if this is a range type, create the LLVM type
        else if (type->IsRange())
        {
            vector<Type*> members;
            for (const MemberInfo* memberInfo : type->GetMembers())
            {
                Type* memberType = GetType(memberInfo->GetType());
                if (memberType == nullptr)
                {
                    return nullptr;
                }

                members.push_back(memberType);
            }

            llvmType = StructType::create(context, members, llvmName);

            // register type so we don't have to create it again
            types.insert({llvmName, llvmType});
        }
        else if (type->IsArray())
        {
            Type* innerType = GetType(type->GetInnerType());
            if (innerType != nullptr)
            {
                Type* arrayStructElements[2];
                arrayStructElements[0] = GetType(TypeInfo::GetUIntSizeType());
                arrayStructElements[1] = PointerType::get(innerType, 0);

                ArrayRef<Type*> arrayRef(arrayStructElements, 2);
                llvmType = StructType::create(context, arrayRef, llvmName);

                // register type so we don't have to create it again
                types.insert({llvmName, llvmType});
            }
        }
        else // could not determine the type
        {
            llvmType = nullptr;
        }
    }

    return llvmType;
}

DIType* LlvmIrGenerator::GetDebugType(const TypeInfo* type)
{
    DIType* diType = nullptr;
    if (type->IsInt())
    {
        const string& name = type->GetShortName();
        unsigned numBits = type->GetNumBits();
        unsigned encoding = (type->GetSign() == TypeInfo::eSigned) ? dwarf::DW_ATE_signed : dwarf::DW_ATE_unsigned;
        diType = diBuilder->createBasicType(name, numBits, encoding);
    }
    else if (type->IsBool())
    {
        const string& name = type->GetShortName();
        diType = diBuilder->createBasicType(name, 8, dwarf::DW_ATE_boolean);
    }
    else if (type->IsPointer())
    {
        DIType* innerDiType = GetDebugType(type->GetInnerType());
        if (innerDiType == nullptr)
        {
            return nullptr;
        }
        diType = diBuilder->createPointerType(innerDiType, TypeInfo::GetPointerSize(), 0, llvm::None, type->GetShortName());
    }
    else if (type->IsSameAs(*TypeInfo::UnitType))
    {
        diType = diBuilder->createBasicType(TypeInfo::UnitType->GetShortName(), 0, dwarf::DW_ATE_unsigned);
    }
    else if (type->IsSameAs(*TypeInfo::GetStringType()))
    {
        const TypeInfo* strType = TypeInfo::GetStringType();
        const string& name = strType->GetShortName();
        unsigned numBits = strType->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : strType->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = GetDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            const string& memberName = member->GetName();
            unsigned size = memberType->GetNumBits();
            // TODO: don't hard-code alignment
            elements.push_back(diBuilder->createMemberType(nullptr, memberName, nullptr, 0, size, 4, offset, DINode::FlagZero, memberDiType));

            offset += size;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        diType = diBuilder->createStructType(nullptr, name, nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else if (type->IsArray())
    {
        const string& name = type->GetShortName();
        unsigned numBits = type->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : type->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = GetDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            const string& memberName = member->GetName();
            unsigned size = memberType->GetNumBits();
            // TODO: don't hard-code alignment
            elements.push_back(diBuilder->createMemberType(nullptr, memberName, nullptr, 0, size, 4, offset, DINode::FlagZero, memberDiType));

            offset += size;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
        diType = diBuilder->createStructType(nullptr, name, nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else if (type->IsRange())
    {
        const string& name = type->GetShortName();
        unsigned numBits = type->GetNumBits();

        SmallVector<Metadata*, 2> elements;

        uint64_t offset = 0;
        for (const MemberInfo* member : type->GetMembers())
        {
            const TypeInfo* memberType = member->GetType();
            DIType* memberDiType = GetDebugType(memberType);
            if (memberDiType == nullptr)
            {
                return nullptr;
            }

            const string& memberName = member->GetName();
            uint64_t memberSize = memberDiType->getSizeInBits();
            // TODO: better way to get alignment?
            uint64_t alignment = (memberSize > 32) ? 32 : memberSize;
            elements.push_back(diBuilder->createMemberType(nullptr, memberName, nullptr, 0, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

            offset += memberSize;
        }

        DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);

        // TODO: set alignment
        diType = diBuilder->createStructType(nullptr, name, nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
    }
    else
    {
        const string& name = type->GetShortName();
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

    return diType;
}

bool LlvmIrGenerator::CreateFunctionDeclaration(const FunctionDeclaration* funcDecl)
{
    // get the return type
    Type* returnType = GetType(funcDecl->returnType);
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
        Type* varType = GetType(declParam->type);
        parameters.push_back(varType);
    }

    FunctionType* funcType = FunctionType::get(returnType, parameters, false);
    Function::Create(funcType, Function::ExternalLinkage, funcDecl->name, module);

    return true;
}

AllocaInst* LlvmIrGenerator::CreateVariableAlloc(Function* function, Type* type, const string& paramName)
{
    // create builder to insert alloca at beginning of function
    BasicBlock& entryBlock = function->getEntryBlock();
    IRBuilder<> tempBuilder(&entryBlock, entryBlock.begin());

    AllocaInst* alloca = tempBuilder.CreateAlloca(type, nullptr, paramName);
    return alloca;
}

Value* LlvmIrGenerator::CreateExt(llvm::Value* value, const TypeInfo* valueType, const TypeInfo* dstType)
{
    Type* llvmType = GetType(dstType);

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
    function->getBasicBlockList().push_back(mergeBlock);
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
        builder.SetCurrentDebugLocation(DebugLoc::get(line, column, diScopes.top()));
    }
}

void LlvmIrGenerator::CreateDebugVariable(const Token* token, const TypeInfo* type, AllocaInst* alloca)
{
    if (dbgInfo)
    {
        unsigned line = token->line;
        unsigned column = token->column;
        DIType* varDebugType = GetDebugType(type);
        if (varDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        DIScope* diScope = diScopes.top();
        DILocalVariable* diVar = diBuilder->createAutoVariable(diScope, token->value, currentDiFile, line, varDebugType, true);
        diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DebugLoc::get(line, column, diScope), builder.GetInsertBlock());
    }
}
