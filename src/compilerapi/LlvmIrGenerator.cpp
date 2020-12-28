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

LlvmIrGenerator::LlvmIrGenerator(const Config& config, ErrorLogger& logger) :
    targetMachine(config.targetMachine),
    inFilename(config.inFilename),
    optimizationLevel(config.optimizationLevel),
    dbgInfo(config.debugInfo),
    boundsCheck(config.boundsCheck),
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
    Expression* subExpr = unaryExpression->GetSubExpression();

    subExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* subExprValue = resultValue;

    SetDebugLocation(unaryExpression->GetOperatorToken());

    switch (unaryExpression->GetOperator())
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

    ExtendType(subExpr->GetType(), unaryExpression->GetType(), resultValue);
}

void LlvmIrGenerator::Visit(BinaryExpression* binaryExpression)
{
    BinaryExpression::EOperator op = binaryExpression->GetOperator();
    Expression* leftExpr = binaryExpression->GetLeftExpression();
    Expression* rightExpr = binaryExpression->GetRightExpression();

    if (op == BinaryExpression::eLogicalAnd)
    {
        SetDebugLocation(binaryExpression->GetOperatorToken());
        resultValue = CreateLogicalBranch(leftExpr, rightExpr, true);
    }
    else if (op == BinaryExpression::eLogicalOr)
    {
        SetDebugLocation(binaryExpression->GetOperatorToken());
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

        SetDebugLocation(binaryExpression->GetOperatorToken());

        bool isAssignment = BinaryExpression::IsAssignment(op);

        // check if values need to be sign extended
        const TypeInfo* leftType = leftExpr->GetType();
        const TypeInfo* rightType = rightExpr->GetType();

        const TypeInfo* intermediateType = nullptr;
        Value* storeValue = nullptr;
        if (isAssignment)
        {
            ExtendType(rightType, leftType, rightValue);

            storeValue = leftValue;

            // if we are also doing a computation (e.g. +=), we need to load the left value
            if (BinaryExpression::IsComputationAssignment(op))
            {
                leftValue = builder.CreateLoad(leftValue, "load");
            }
        }
        else
        {
            intermediateType = ExtendType(leftType, rightType, leftValue, rightValue);
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
            case BinaryExpression::eInclusiveRange:
            case BinaryExpression::eExclusiveRange:
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
        else
        {
            // if necessary, sign/zero extend the result to match the result type
            ExtendType(intermediateType, binaryExpression->GetType(), resultValue);
        }
    }
}

Value* LlvmIrGenerator::GenerateIntSubscriptIr(const BinaryExpression* binaryExpression, Value* leftValue, Value* rightValue)
{
    const TypeInfo* rightType = binaryExpression->GetRightExpression()->GetType();

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
            const Token* opToken = binaryExpression->GetOperatorToken();

            Constant* fileStrPtr = CreateConstantString(opToken->GetFilename());
            Value* fileStr = builder.CreateLoad(fileStrPtr, "filestr");
            Constant* lineNum = ConstantInt::get(context, APInt(32, opToken->GetLine(), false));
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
    Expression* rightExpr = binaryExpression->GetRightExpression();
    const TypeInfo* rightType = rightExpr->GetType();
    const TypeInfo* startType = rightType->GetMembers()[0]->GetType();
    const TypeInfo* endType = rightType->GetMembers()[1]->GetType();

    // get array size
    Value* size = builder.CreateExtractValue(leftValue, 0, "size");

    // get array data pointer
    Value* data = builder.CreateExtractValue(leftValue, 1, "data");

    // get range start
    Value* start = builder.CreateExtractValue(rightValue, 0, "start");
    if (startType->GetNumBits() < uIntSizeNumBits)
    {
        Type* extType = GetType(TypeInfo::GetUIntSizeType());
        start = builder.CreateZExt(start, extType, "zeroext");
    }

    // get range end
    Value* end = builder.CreateExtractValue(rightValue, 1, "end");
    if (endType->GetNumBits() < uIntSizeNumBits)
    {
        Type* extType = GetType(TypeInfo::GetUIntSizeType());
        end = builder.CreateZExt(end, extType, "zeroext");
    }

    // if this range is inclusive, add 1 to the end
    if (!rightType->IsExclusive())
    {
        Value* one = ConstantInt::get(context, APInt(uIntSizeNumBits, 1, false));
        end = builder.CreateAdd(end, one, "add");
    }

    // check end
    Value* endOk = builder.CreateICmpULT(end, size, "endok");
    Value* checkEnd = builder.CreateSelect(endOk, end, size, "checkend");

    // calculate new size
    Value* newSize = builder.CreateSub(checkEnd, start, "sub");

    vector<Value*> indices;
    indices.push_back(start);
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
    whileLoop->GetCondition()->Accept(this);
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
    whileLoop->GetExpression()->Accept(this);
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
    Expression* iterExpr = forLoop->GetIterExpression();
    iterExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* iterableValue = resultValue;

    const TypeInfo* iterableType = iterExpr->GetType();
    bool isRangeLoop = iterableType->IsRange();
    bool isArrayLoop = iterableType->IsArray();
    const TypeInfo* iterableInnerType = nullptr;
    if (isRangeLoop)
    {
        assert(iterableType->GetMemberCount() > 0 && "For loop iterator expression type does not have members");
        iterableInnerType = iterableType->GetMembers()[0]->GetType();
    }
    else if (isArrayLoop)
    {
        iterableInnerType = iterableType->GetInnerType();
    }
    else
    {
        assert(false && "Invalid for loop iterable type");
    }

    // create new scope for iterator
    Scope scope(symbolTable);

    const Token* varNameToken = forLoop->GetVariableNameToken();
    DILexicalBlock* diBlock = nullptr;
    if (dbgInfo)
    {
        // TODO: it would probably be better to use the 'for' token as the scope start
        DIScope* currentScope = diScopes.top();
        unsigned line = varNameToken->GetLine();
        unsigned column = varNameToken->GetColumn();
        diBlock = diBuilder->createLexicalBlock(currentScope, diFile, line, column);
    }
    DebugScope dbgScope(dbgInfo, diScopes, diBlock);

    // create iterator
    const string& varName = forLoop->GetVariableName();
    const TypeInfo* varType = forLoop->GetVariableType();
    bool isSigned = varType->GetSign() == TypeInfo::eSigned;
    Type* type = GetType(varType);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown variable declaration type");
        return;
    }
    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, varType, alloca);
    CreateDebugVariable(varNameToken, varType, alloca);

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
    PHINode* iter = builder.CreatePHI(iterType, 2, "iter");
    iter->addIncoming(startValue, incomingBlock);
    CmpInst::Predicate predicate = CmpInst::BAD_ICMP_PREDICATE;
    if (isRangeLoop)
    {
        if (iterableType->IsExclusive())
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
    }

    // generate loop body IR
    LoopInfo loopInfo;
    loopInfo.breakBlock = loopExitBlock;
    loopInfo.continueBlock = loopIterBlock;
    loops.push(loopInfo);
    forLoop->GetExpression()->Accept(this);
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
    LoopControl::EControlType controlType = loopControl->GetControlType();
    LoopInfo loopInfo = loops.top();

    SetDebugLocation(loopControl->GetToken());

    if (controlType == LoopControl::eBreak)
    {
        builder.CreateBr(loopInfo.breakBlock);
    }
    else if (controlType == LoopControl::eContinue)
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
    ss << "after" << loopControl->GetToken()->GetValue();
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

    // sign extend return value if needed
    const TypeInfo* expressionType = ret->expression->GetType();
    const TypeInfo* returnType = currentFunctionDefinition->GetDeclaration()->GetReturnType();
    ExtendType(expressionType, returnType, resultValue);

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
    const FunctionDeclaration* declaration = functionDefinition->GetDeclaration();
    const string& funcName = declaration->GetName();
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

    const Parameters& params = declaration->GetParameters();

    DISubprogram* diSubprogram = nullptr;
    if (dbgInfo)
    {
        SmallVector<Metadata*, 8> funTypes;
        DIType* retDebugType = GetDebugType(declaration->GetReturnType());
        if (retDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        funTypes.push_back(retDebugType);

        for (const Parameter* param : params)
        {
            DIType* paramDebugType = GetDebugType(param->GetType());
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            funTypes.push_back(paramDebugType);
        }

        DISubroutineType* subroutine = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(funTypes), DINode::FlagPrototyped);

        unsigned line = declaration->GetNameToken()->GetLine();
        diSubprogram = diBuilder->createFunction(diFile, funcName, "", diFile, line, subroutine, 0, DINode::FlagZero, DISubprogram::SPFlagDefinition);
        func->setSubprogram(diSubprogram);

        // unset debug location for function prolog
        builder.SetCurrentDebugLocation(DebugLoc());
    }

    DebugScope dbgScope(dbgInfo, diScopes, diSubprogram);

    size_t idx = 0;
    for (Argument& arg : func->args())
    {
        const Parameter* param = params[idx];
        const string& paramName = param->GetName();
        arg.setName(paramName);
        AllocaInst* alloca = CreateVariableAlloc(func, arg.getType(), paramName);
        builder.CreateStore(&arg, alloca);
        const TypeInfo* paramType = param->GetType();
        symbolTable.AddVariable(paramName, paramType, alloca);

        if (dbgInfo)
        {
            const Token* token = param->GetNameToken();
            unsigned line = token->GetLine();
            DIType* paramDebugType = GetDebugType(paramType);
            if (paramDebugType == nullptr)
            {
                resultValue = nullptr;
                return;
            }
            DILocalVariable* diVar = diBuilder->createParameterVariable(diSubprogram, paramName, idx + 1, diFile, line, paramDebugType, true);
            diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DebugLoc::get(line, 0, diSubprogram), builder.GetInsertBlock());
        }

        ++idx;
    }

    // process function body expression
    currentFunctionDefinition = functionDefinition;
    currentFunction = func;
    Expression* expression = functionDefinition->GetExpression();
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
        // sign extend return value if needed
        const TypeInfo* expressionType = expression->GetType();
        const TypeInfo* returnType = declaration->GetReturnType();
        ExtendType(expressionType, returnType, resultValue);

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
    const string& structName = structDefinition->GetName();
    const TypeInfo* typeInfo = structDefinition->GetType();

    vector<Type*> members;
    for (const MemberDefinition* memberDef : structDefinition->GetMembers())
    {
        const MemberInfo* memberInfo = typeInfo->GetMember(memberDef->GetName());
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

        DIFile* file = diFile;

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
            unsigned memberLine = member->GetToken()->GetLine();
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
    for (const MemberInitialization* member : structInitializationExpression->GetMemberInitializations())
    {
        Expression* expr = member->GetExpression();
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        SetDebugLocation(member->GetNameToken());

        const MemberInfo* memberInfo = typeInfo->GetMember(member->GetName());

        ExtendType(expr->GetType(), memberInfo->GetType(), resultValue);

        index[0] = memberInfo->GetIndex();
        initValue = builder.CreateInsertValue(initValue, resultValue, index, "agg");
    }

    resultValue = initValue;
}

void LlvmIrGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    // add all struct names to types map
    for (StructDefinition* structDef : moduleDefinition->GetStructDefinitions())
    {
        const string& structName = structDef->GetName();
        StructType* structType = StructType::create(context, structName);
        types.insert({structName, structType});

        if (dbgInfo)
        {
            const AggregateType* aggType = dynamic_cast<const AggregateType*>(structDef->GetType());

            DIFile* file = diFile;
            const string& name = aggType->GetShortName();
            unsigned numBits = aggType->GetNumBits();

            unsigned line = aggType->GetToken()->GetLine();
            // TODO: set alignment
            SmallVector<Metadata*, 0> elements;
            DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);
            DICompositeType* diType = diBuilder->createStructType(file, name, file, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
            diStructTypes.insert({structName, diType});
        }
    }

    // generate struct declarations
    for (StructDefinition* structDef : moduleDefinition->GetStructDefinitions())
    {
        structDef->Accept(this);
    }

    // create function declarations

    for (ExternFunctionDeclaration* externFunc : moduleDefinition->GetExternFunctionDeclarations())
    {
        const FunctionDeclaration* decl = externFunc->GetDeclaration();
        bool ok = CreateFunctionDeclaration(decl);
        if (!ok)
        {
            resultValue = nullptr;
            return;
        }
    }

    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        const FunctionDeclaration* decl = funcDef->GetDeclaration();
        bool ok = CreateFunctionDeclaration(decl);
        if (!ok)
        {
            resultValue = nullptr;
            return;
        }
    }

    // generate code for functions
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        funcDef->Accept(this);
        if (resultValue == nullptr)
        {
            return;
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
    TypeInfo::ESign sign = type->GetSign();
    if (sign == TypeInfo::eSigned)
    {
        numBits = numericExpression->GetMinSignedSize();
        isSigned = true;
    }
    else if (sign == TypeInfo::eUnsigned || sign == TypeInfo::eContextDependent)
    {
        numBits = numericExpression->GetMinUnsignedSize();
        isSigned = false;
    }
    else
    {
        logger.LogInternalError("Unexpected numeric expression sign");
        resultValue = nullptr;
        return;
    }

    SetDebugLocation(numericExpression->GetToken());

    int64_t value = numericExpression->GetValue();
    resultValue = ConstantInt::get(context, APInt(numBits, value, isSigned));
}

void LlvmIrGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    SetDebugLocation(boolLiteralExpression->GetToken());

    bool value = boolLiteralExpression->GetValue();
    resultValue = value ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
}

void LlvmIrGenerator::Visit(StringLiteralExpression* stringLiteralExpression)
{
    const vector<char>& chars = stringLiteralExpression->GetCharacters();
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

void LlvmIrGenerator::Visit(VariableExpression* variableExpression)
{
    const string& name = variableExpression->GetName();
    AllocaInst* alloca = symbolTable.GetValue(name);
    if (alloca == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("No alloca found for '{}'", name);
        return;
    }

    SetDebugLocation(variableExpression->GetToken());

    Expression::EAccessType accessType = variableExpression->GetAccessType();
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

void LlvmIrGenerator::Visit(ArraySizeValueExpression* arrayExpression)
{
    unsigned uIntSizeNumBits = TypeInfo::GetUIntSizeType()->GetNumBits();

    NumericExpression* sizeExpression = dynamic_cast<NumericExpression*>(arrayExpression->sizeExpression);
    assert(sizeExpression != nullptr);

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
    uint64_t arraySize = (uint64_t)sizeExpression->GetValue();

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
    ExtendType(valueExpression->GetType(), innerTypeInfo, valueValue);

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
    const TypeInfo* innerTypeInfo = typeInfo->GetInnerType();
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
        ExtendType(expr->GetType(), innerTypeInfo, resultValue);

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
        const Token* startToken = blockExpression->GetStartToken();
        unsigned line = startToken->GetLine();
        unsigned column = startToken->GetColumn();
        diBlock = diBuilder->createLexicalBlock(currentScope, diFile, line, column);
    }
    DebugScope dbgScope(dbgInfo, diScopes, diBlock);

    const Expressions& expressions = blockExpression->GetExpressions();
    size_t size = expressions.size();

    if (size == 0)
    {
        resultValue = nullptr;
        logger.LogInternalError("Block expression has no sub-expressions");
    }
    else
    {
        for (Expression* expression : expressions)
        {
            expression->Accept(this);
            if (resultValue == nullptr)
            {
                break;
            }
        }

        // the block expression's result is the result of its last expression
    }
}

void LlvmIrGenerator::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    Function* func = module->getFunction(funcName);
    if (func == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Use of undeclared function '{}'", funcName);
        return;
    }

    if (functionExpression->GetArguments().size() != func->arg_size())
    {
        resultValue = nullptr;
        logger.LogInternalError("Unexpected number of function arguments");
        return;
    }

    const FunctionDeclaration* funcDecl = functionExpression->GetFunctionDeclaration();
    const Parameters& declParams = funcDecl->GetParameters();
    vector<Expression*> argExpressions = functionExpression->GetArguments();
    vector<Value*> args;
    for (size_t i = 0; i < argExpressions.size(); ++i)
    {
        Expression* expr = argExpressions[i];
        expr->Accept(this);
        if (resultValue == nullptr)
        {
            return;
        }

        // sign extend arg value if needed
        const TypeInfo* argType = expr->GetType();
        const TypeInfo* paramType = declParams[i]->GetType();
        ExtendType(argType, paramType, resultValue);

        args.push_back(resultValue);
    }

    SetDebugLocation(functionExpression->GetNameToken());

    resultValue = builder.CreateCall(func, args, "call");
}

void LlvmIrGenerator::Visit(MemberExpression* memberExpression)
{
    Expression* expr = memberExpression->GetSubExpression();
    expr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    SetDebugLocation(memberExpression->GetMemberNameToken());

    const TypeInfo* type = expr->GetType();
    bool isPointer = type->IsPointer();
    if (isPointer)
    {
        type = type->GetInnerType();
    }

    const string& memberName = memberExpression->GetMemberName();
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
    branchExpression->GetIfCondition()->Accept(this);
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

    Expression* ifExpr = branchExpression->GetIfExpression();
    ifExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    // extend type if necessary
    ExtendType(ifExpr->GetType(), resultType, resultValue);

    Value* ifExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "true" block
    trueBlock = builder.GetInsertBlock();

    // generate "false" block IR
    function->getBasicBlockList().push_back(falseBlock);
    builder.SetInsertPoint(falseBlock);

    Expression* elseExpr = branchExpression->GetElseExpression();
    elseExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    // extend type if necessary
    ExtendType(elseExpr->GetType(), resultType, resultValue);

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

void LlvmIrGenerator::Visit(VariableDeclaration* variableDeclaration)
{
    const string& varName = variableDeclaration->GetName();
    const TypeInfo* varType = variableDeclaration->GetVariableType();
    Type* type = GetType(varType);
    if (type == nullptr)
    {
        resultValue = nullptr;
        logger.LogInternalError("Unknown variable declaration type");
        return;
    }

    AllocaInst* alloca = CreateVariableAlloc(currentFunction, type, varName);
    symbolTable.AddVariable(varName, varType, alloca);
    CreateDebugVariable(variableDeclaration->GetNameToken(), varType, alloca);

    variableDeclaration->GetAssignmentExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    // variable declaration expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

bool LlvmIrGenerator::Generate(SyntaxTreeNode* syntaxTree, Module*& module)
{
    module = new Module(inFilename, context);
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

        bool isOptimized = optimizationLevel > 0;
        diFile = diBuilder->createFile(inFilename, fs::current_path().string());
        diBuilder->createCompileUnit(dwarf::DW_LANG_C, diFile, "WIP Compiler", isOptimized, "", 0);

        diScopes.push(diFile);
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
        unsigned memberNumBits = type->GetMembers()[0]->GetType()->GetNumBits();
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
    Type* returnType = GetType(funcDecl->GetReturnType());
    if (returnType == nullptr)
    {
        logger.LogInternalError("Invalid function return type");
        return false;
    }

    // get the parameter types
    const Parameters& declParams = funcDecl->GetParameters();
    vector<Type*> parameters;
    parameters.reserve(declParams.size());
    for (const Parameter* declParam : declParams)
    {
        Type* varType = GetType(declParam->GetType());
        parameters.push_back(varType);
    }

    FunctionType* funcType = FunctionType::get(returnType, parameters, false);
    Function::Create(funcType, Function::ExternalLinkage, funcDecl->GetName(), module);

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

const TypeInfo* LlvmIrGenerator::ExtendType(const TypeInfo* srcType, const TypeInfo* dstType, Value*& value)
{
    const TypeInfo* resultType = nullptr;

    // sign extend int value if needed
    if (srcType->IsInt() && dstType->IsInt() && srcType->GetNumBits() < dstType->GetNumBits())
    {
        value = CreateExt(value, srcType, dstType);
        resultType = dstType;
    }
    // sign extend range value if needed
    else if (srcType->IsRange() && dstType->IsRange() && srcType->GetNumBits() < dstType->GetNumBits())
    {
        const vector<const MemberInfo*>& srcMembers = srcType->GetMembers();
        const NumericLiteralType* srcLeftIntLit = dynamic_cast<const NumericLiteralType*>(srcMembers[0]->GetType());
        const NumericLiteralType* srcRightIntLit = dynamic_cast<const NumericLiteralType*>(srcMembers[1]->GetType());
        if (srcLeftIntLit != nullptr && srcRightIntLit != nullptr)
        {
            Type* dstRangeType = GetType(dstType);
            if (dstRangeType == nullptr)
            {
                logger.LogInternalError("Unknown range type");
                return nullptr;
            }
            const vector<const MemberInfo*> dstMembers = dstType->GetMembers();

            vector<unsigned> index(1);
            Value* initValue = UndefValue::get(dstRangeType);

            // extend start
            index[0] = 0;
            Value* leftValue = builder.CreateExtractValue(value, index);
            leftValue = CreateExt(leftValue, srcLeftIntLit, dstMembers[0]->GetType());
            initValue = builder.CreateInsertValue(initValue, leftValue, index);

            // extend end
            index[0] = 1;
            Value* rightValue = builder.CreateExtractValue(value, index);
            rightValue = CreateExt(rightValue, srcRightIntLit, dstMembers[1]->GetType());
            initValue = builder.CreateInsertValue(initValue, rightValue, index);

            value = initValue;
            resultType = dstType;
        }
        else
        {
            resultType = srcType;
        }
    }
    else
    {
        resultType = srcType;
    }

    return resultType;
}

const TypeInfo* LlvmIrGenerator::ExtendType(const TypeInfo* leftType, const TypeInfo* rightType, Value*& leftValue, Value*& rightValue)
{
    const TypeInfo* resultType = nullptr;

    if (leftType->IsInt() && rightType->IsInt() && leftType->GetNumBits() != rightType->GetNumBits())
    {
        if (leftType->GetNumBits() < rightType->GetNumBits())
        {
            leftValue = CreateExt(leftValue, leftType, rightType);
            resultType = rightType;
        }
        else
        {
            rightValue = CreateExt(rightValue, rightType, leftType);
            resultType = leftType;
        }
    }
    else
    {
        // if the left and right types are not differnet size ints, then they
        // should be the same, so just pick one to return
        resultType = leftType;
    }

    return resultType;
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
        unsigned line = token->GetLine();
        unsigned column = token->GetColumn();
        builder.SetCurrentDebugLocation(DebugLoc::get(line, column, diScopes.top()));
    }
}

void LlvmIrGenerator::CreateDebugVariable(const Token* token, const TypeInfo* type, AllocaInst* alloca)
{
    if (dbgInfo)
    {
        const string& varName = token->GetValue();
        unsigned line = token->GetLine();
        unsigned column = token->GetColumn();
        DIType* varDebugType = GetDebugType(type);
        if (varDebugType == nullptr)
        {
            resultValue = nullptr;
            return;
        }
        DIScope* diScope = diScopes.top();
        DILocalVariable* diVar = diBuilder->createAutoVariable(diScope, varName, diFile, line, varDebugType, true);
        diBuilder->insertDeclare(alloca, diVar, diBuilder->createExpression(), DebugLoc::get(line, column, diScope), builder.GetInsertBlock());
    }
}
