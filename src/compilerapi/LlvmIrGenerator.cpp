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
    logger(logger),
    builder(context),
    diBuilder(nullptr),
    module(nullptr),
    currentFunction(nullptr),
    resultValue(nullptr),
    unitType(nullptr),
    strStructType(nullptr),
    strPointerType(nullptr),
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
    builder.SetInsertPoint(loopCondBlock);
    builder.CreateCondBr(conditionResult, loopBodyBlock, loopExitBlock);

    // set insert point to the loop body block
    builder.SetInsertPoint(loopBodyBlock);

    // generate loop body IR
    whileLoop->GetExpression()->Accept(this);
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
    Value* rangeValue = resultValue;
    const TypeInfo* rangeType = iterExpr->GetType();
    assert(rangeType->GetMemberCount() > 0 && "For loop iterator expression type does not have members");
    const TypeInfo* rangeMemberType = rangeType->GetMembers()[0]->GetType();

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

    vector<unsigned> index(1);

    // set iterator to range start
    index[0] = 0;
    Value* startValue = builder.CreateExtractValue(rangeValue, index, "start");
    startValue = CreateExt(startValue, rangeMemberType, varType);
    builder.CreateStore(startValue, alloca);

    // get range end
    index[0] = 1;
    Value* endValue = builder.CreateExtractValue(rangeValue, index, "end");
    endValue = CreateExt(endValue, rangeMemberType, varType);

    Function* function = builder.GetInsertBlock()->getParent();

    BasicBlock* loopCondBlock = BasicBlock::Create(context, "forCond", function);

    // create unconditional branch from current block to loop condition block
    builder.CreateBr(loopCondBlock);

    // set insert point to the loop condition block
    builder.SetInsertPoint(loopCondBlock);

    SetDebugLocation(varNameToken);

    // generate the condition IR
    Value* iter = builder.CreateLoad(alloca, "iter");
    CmpInst::Predicate predicate = CmpInst::BAD_ICMP_PREDICATE;
    if (rangeType->IsExclusive())
    {
        predicate = isSigned ? CmpInst::ICMP_SLT : CmpInst::ICMP_ULT;
    }
    else
    {
        predicate = isSigned ? CmpInst::ICMP_SLE : CmpInst::ICMP_ULE;
    }
    Value* conditionResult = builder.CreateICmp(predicate, iter, endValue, "cmp");

    BasicBlock* loopBodyBlock = BasicBlock::Create(context, "forBody", function);
    BasicBlock* loopExitBlock = BasicBlock::Create(context, "forExit");

    // create conditional branch from condition block to either the loop body
    // or the loop exit
    builder.SetInsertPoint(loopCondBlock);
    builder.CreateCondBr(conditionResult, loopBodyBlock, loopExitBlock);

    // set insert point to the loop body block
    builder.SetInsertPoint(loopBodyBlock);

    // generate loop body IR
    forLoop->GetExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    // increment iterator
    iter = builder.CreateLoad(alloca, "iter");
    Value* one = ConstantInt::get(type, 1, isSigned);
    iter = builder.CreateAdd(iter, one, "inc");
    builder.CreateStore(iter, alloca);

    // create unconditional branch to loop condition block
    builder.CreateBr(loopCondBlock);

    // add exit block to function
    loopExitBlock->insertInto(function);

    // set insert point to the loop exit block
    builder.SetInsertPoint(loopExitBlock);

    // for loop expressions always evaluate to the unit type
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
    currentFunction = func;
    Expression* expression = functionDefinition->GetExpression();
    expression->Accept(this);
    currentFunction = nullptr;

    if (resultValue == nullptr)
    {
        return;
    }

    // sign extend return value if needed
    const TypeInfo* expressionType = expression->GetType();
    const TypeInfo* returnType = declaration->GetReturnType();
    ExtendType(expressionType, returnType, resultValue);

    builder.CreateRet(resultValue);

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

    StructType* structType = StructType::create(context, members, structName);
    types.insert({structName, structType});
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

    // if we already have a constant for this string, then reuse it
    auto iter = strings.find(chars);
    if (iter != strings.end())
    {
        resultValue = iter->second;
    }
    else // create a new string constant
    {
        const TypeInfo* sizeType = TypeInfo::GetUIntSizeType();

        vector<Constant*> initValues =
        {
            ConstantInt::get(context, APInt(sizeType->GetNumBits(), chars.size(), false)),
            ConstantDataArray::getString(context, StringRef(chars.data(), chars.size()), false),
        };
        Constant* initializer = ConstantStruct::getAnon(context, initValues);

        // create name
        stringstream structName;
        structName << "strStruct" << globalStringCounter;
        ++globalStringCounter;

        module->getOrInsertGlobal(structName.str(), initializer->getType());
        GlobalVariable* globalStruct = module->getNamedGlobal(structName.str());
        globalStruct->setConstant(true);
        globalStruct->setInitializer(initializer);

        Constant* constant = ConstantExpr::getBitCast(globalStruct, strPointerType);

        strings[chars] = constant;

        resultValue = constant;
    }
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
        case Expression::eLoad:
            resultValue = builder.CreateLoad(alloca, name);
            break;
        case Expression::eStore:
            resultValue = alloca;
            break;
    }
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

    const TypeInfo* exprType = expr->GetType();
    const string& memberName = memberExpression->GetMemberName();
    const MemberInfo* member = exprType->GetMember(memberName);
    if (member == nullptr)
    {
        logger.LogInternalError("Member is null");
        resultValue = nullptr;
        return;
    }
    unsigned memberIndex = member->GetIndex();

    Expression::EAccessType accessType = memberExpression->GetAccessType();

    if (accessType == Expression::eStore || resultValue->getType()->isPointerTy())
    {
        vector<Value*> indices;
        indices.push_back(ConstantInt::get(context, APInt(TypeInfo::GetUIntSizeType()->GetNumBits(), 0)));
        indices.push_back(ConstantInt::get(context, APInt(32, memberIndex)));

        // calculate member address
        Value* memberPointer = builder.CreateInBoundsGEP(resultValue, indices, "mber");

        if (accessType == Expression::eLoad)
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
    strStructElements[1] = ArrayType::get(Type::getInt8Ty(context), 0);

    ArrayRef<Type*> strArrayRef(strStructElements, STR_STRUCT_ELEMENTS_SIZE);
    strStructType = StructType::create(context, strArrayRef, "str");
    strPointerType = strStructType->getPointerTo(addressSpace);

    // register types
    types.insert({TypeInfo::BoolType->GetShortName(), boolType});
    types.insert({TypeInfo::GetStringPointerType()->GetShortName(), strPointerType});

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

string createRangeName(const TypeInfo* rangeType)
{
    unsigned memberNumBits = rangeType->GetMembers()[0]->GetType()->GetNumBits();
    string name = "Range" + to_string(memberNumBits);
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
    else
    {
        // get the LLVM type name
        string llvmName;
        bool isRange = type->IsRange();
        if (isRange)
        {
            llvmName = createRangeName(type);
        }
        else
        {
            llvmName = type->GetShortName();
        }

        // try to lookup this type to see if it's already been created
        auto iter = types.find(llvmName);
        if (iter != types.end())
        {
            llvmType = iter->second;
        }
        // if this is a range type, create the LLVM type
        else if (isRange)
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
    else if (type->IsSameAs(*TypeInfo::UnitType))
    {
        diType = diBuilder->createBasicType(TypeInfo::UnitType->GetShortName(), 0, dwarf::DW_ATE_unsigned);
    }
    else if (type->IsSameAs(*TypeInfo::GetStringPointerType()))
    {
        // TODO: add debug info for data member

        const TypeInfo* strType = TypeInfo::GetStringPointerType();
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
        DIType* structType = diBuilder->createStructType(nullptr, name + "_data", nullptr, 0, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
        diType = diBuilder->createPointerType(structType, TypeInfo::GetPointerSize(), 0, llvm::None, name);
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
        const AggregateType* aggType = dynamic_cast<const AggregateType*>(type);

        if (aggType != nullptr)
        {
            DIFile* file = diFile;
            const string& name = aggType->GetShortName();
            unsigned numBits = aggType->GetNumBits();

            SmallVector<Metadata*, 8> elements;

            uint64_t offset = 0;
            for (const MemberInfo* member : aggType->GetMembers())
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
                unsigned memberLine = member->GetToken()->GetLine();
                elements.push_back(diBuilder->createMemberType(file, memberName, file, memberLine, memberSize, alignment, offset, DINode::FlagZero, memberDiType));

                offset += memberSize;
            }

            DINodeArray elementsArray = diBuilder->getOrCreateArray(elements);

            unsigned line = aggType->GetToken()->GetLine();
            // TODO: set alignment
            diType = diBuilder->createStructType(file, name, file, line, numBits, 0, DINode::FlagZero, nullptr, elementsArray);
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
