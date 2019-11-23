#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "LlvmIrGenerator.h"
#include "Config.h"
#include "SyntaxTree.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Target/TargetMachine.h"
#include "utils.h"
#include <iostream>
#pragma clang diagnostic pop

using namespace llvm;
using namespace std;
using namespace SyntaxTree;

LlvmIrGenerator::LlvmIrGenerator(const Config& config) :
    targetMachine(config.targetMachine),
    builder(context),
    module(nullptr),
    resultValue(nullptr)
{
    llvm::ArrayRef<Type*> emptyArray;
    unitType = StructType::create(context, emptyArray, "UnitType");
}

void LlvmIrGenerator::Visit(SyntaxTree::UnaryExpression* unaryExpression)
{
    unaryExpression->GetSubExpression()->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }
    Value* subExprValue = resultValue;

    switch (unaryExpression->GetOperator())
    {
        case UnaryExpression::eNegative:
            resultValue = builder.CreateNeg(subExprValue, "neg");
            break;
        case UnaryExpression::eComplement:
            resultValue = builder.CreateNot(subExprValue, "not");
            break;
    }
}

void LlvmIrGenerator::Visit(BinaryExpression* binaryExpression)
{
    BinaryExpression::EOperator op = binaryExpression->GetOperator();
    Expression* leftExpr = binaryExpression->GetLeftExpression();
    Expression* rightExpr = binaryExpression->GetRightExpression();

    if (op == BinaryExpression::eLogicalAnd)
    {
        resultValue = CreateLogicalAnd(leftExpr, rightExpr);
    }
    else if (op == BinaryExpression::eLogicalOr)
    {
        resultValue = CreateLogicalOr(leftExpr, rightExpr);
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

        // check if values need to be sign extended
        const TypeInfo* leftType = leftExpr->GetType();
        const TypeInfo* rightType = rightExpr->GetType();
        ExtendType(leftType, rightType, leftValue, rightValue);
        bool isSigned = leftType->IsSigned();

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
                resultValue = builder.CreateAdd(leftValue, rightValue, "add");
                break;
            case BinaryExpression::eSubtract:
                resultValue = builder.CreateSub(leftValue, rightValue, "sub");
                break;
            case BinaryExpression::eMultiply:
                resultValue = builder.CreateMul(leftValue, rightValue, "mul");
                break;
            case BinaryExpression::eDivide:
                resultValue = isSigned ? builder.CreateSDiv(leftValue, rightValue, "div") : builder.CreateUDiv(leftValue, rightValue, "div");
                break;
            case BinaryExpression::eRemainder:
                resultValue = isSigned ? builder.CreateSRem(leftValue, rightValue, "rem") : builder.CreateURem(leftValue, rightValue, "rem");
                break;
            case BinaryExpression::eShiftLeft:
                resultValue = builder.CreateShl(leftValue, rightValue, "shl");
                break;
            case BinaryExpression::eShiftRightArithmetic:
                resultValue = builder.CreateAShr(leftValue, rightValue, "ashr");
                break;
            case BinaryExpression::eBitwiseAnd:
                resultValue = builder.CreateAnd(leftValue, rightValue, "bitand");
                break;
            case BinaryExpression::eBitwiseXor:
                resultValue = builder.CreateXor(leftValue, rightValue, "bitxor");
                break;
            case BinaryExpression::eBitwiseOr:
                resultValue = builder.CreateOr(leftValue, rightValue, "bitor");
                break;
            case BinaryExpression::eLogicalAnd:
            case BinaryExpression::eLogicalOr:
                cerr << "Internal error: logical AND and logical OR operators should have been handled before here\n";
                resultValue = nullptr;
                break;
        }
    }
}

void LlvmIrGenerator::Visit(Assignment* assignment)
{
    Expression* expression = assignment->GetExpression();
    expression->Accept(this);
    if (resultValue == nullptr)
    {
        return;
    }

    const string& name = assignment->GetVariableName();
    AllocaInst* alloca = symbolTable.GetValue(name);
    if (alloca == nullptr)
    {
        resultValue = nullptr;
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }

    // sign extend expression value if needed
    const TypeInfo* expressionType = expression->GetType();
    const TypeInfo* varType = symbolTable.GetVariable(name)->GetType();
    ExtendType(expressionType, varType, resultValue);

    builder.CreateStore(resultValue, alloca);

    // assignment expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
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

    BasicBlock* loopExitBlock = BasicBlock::Create(context, "whileExit", function);

    // create conditional branch from condition block to either the loop body
    // or the loop exit
    builder.SetInsertPoint(loopCondBlock);
    builder.CreateCondBr(conditionResult, loopBodyBlock, loopExitBlock);

    // set insert point to the loop exit block
    builder.SetInsertPoint(loopExitBlock);

    // while loop expressions always evaluate to the unit type
    resultValue = ConstantStruct::get(unitType);
}

void LlvmIrGenerator::Visit(FunctionDefinition* functionDefinition)
{
    const FunctionDeclaration* declaration = functionDefinition->GetDeclaration();
    const string& funcName = declaration->GetName();
    Function* func = module->getFunction(funcName);
    if (func == nullptr)
    {
        cerr << "Internal error: Function '" << funcName << "' was not declared\n";
        resultValue = nullptr;
        return;
    }

    if (!func->empty())
    {
        cerr << "Cannot redefine function: '" << funcName << "'\n";
        resultValue = nullptr;
        return;
    }

    // create entry block
    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(basicBlock);

    symbolTable.Push();

    size_t idx = 0;
    for (Argument& arg : func->args())
    {
        VariableDefinition* param = declaration->GetParameters()[idx];
        const string& paramName = param->GetName();
        arg.setName(paramName);
        AllocaInst* alloca = CreateVariableAlloc(func, arg.getType(), paramName);
        builder.CreateStore(&arg, alloca);
        symbolTable.AddVariable(paramName, param, alloca);

        ++idx;
    }

    for (VariableDefinition* varDef : functionDefinition->GetVariableDefinitions())
    {
        const string& varName = varDef->GetName();
        Type* type = GetType(varDef->GetType());
        AllocaInst* alloca = CreateVariableAlloc(func, type, varName);
        symbolTable.AddVariable(varName, varDef, alloca);
    }

    // process return expression
    Expression* expression = functionDefinition->GetExpression();
    expression->Accept(this);

    symbolTable.Pop();

    if (resultValue == nullptr)
    {
        return;
    }

    // sign extend return value if needed
    const TypeInfo* expressionType = expression->GetType();
    const TypeInfo* returnType = declaration->GetReturnType();
    ExtendType(expressionType, returnType, resultValue);

    builder.CreateRet(resultValue);

    bool error = verifyFunction(*func, &errs());
    if (error)
    {
        resultValue = nullptr;
        cerr << "Internal error verifying function\n";
        return;
    }
}

void LlvmIrGenerator::Visit(ModuleDefinition* moduleDefinition)
{
    // create function declarations and build function look-up table
    functions.clear();
    for (FunctionDefinition* funcDef : moduleDefinition->GetFunctionDefinitions())
    {
        functions.insert({funcDef->GetDeclaration()->GetName(), funcDef});

        bool ok = CreateFunctionDeclaration(funcDef->GetDeclaration());
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
    int64_t number = 0;
    bool ok = stringToInteger(numericExpression->GetNumber(), number);
    if (ok)
    {
        unsigned int numBits = numericExpression->GetType()->GetNumBits();
        resultValue = ConstantInt::get(context, APInt(numBits, number, true));
    }
    else
    {
        resultValue = nullptr;
        cerr << "Invalid numeric literal \"" << numericExpression->GetNumber() << "\"\n";
    }
}

void LlvmIrGenerator::Visit(BoolLiteralExpression* boolLiteralExpression)
{
    bool value = false;
    bool ok = stringToBool(boolLiteralExpression->GetValue(), value);
    if (ok)
    {
        resultValue = value ? ConstantInt::getTrue(context) : ConstantInt::getFalse(context);
    }
    else
    {
        resultValue = nullptr;
        cerr << "Invalid boolean literal \"" << boolLiteralExpression->GetValue() << "\"\n";
    }
}

void LlvmIrGenerator::Visit(VariableExpression* variableExpression)
{
    const string& name = variableExpression->GetName();
    AllocaInst* alloca = symbolTable.GetValue(name);
    if (alloca == nullptr)
    {
        resultValue = nullptr;
        cerr << "\"" << name << "\" has not been defined\n";
        return;
    }

    resultValue = builder.CreateLoad(alloca, name);
}

void LlvmIrGenerator::Visit(BlockExpression* blockExpression)
{
    const Expressions& expressions = blockExpression->GetExpressions();
    size_t size = expressions.size();

    if (size == 0)
    {
        resultValue = nullptr;
        cerr << "Internal error: Block expression has no sub-expressions\n";
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

void LlvmIrGenerator::Visit(ExternFunctionDeclaration* externFunctionDeclaration)
{
    // TODO: implement
    cerr << "Internal error: Not implemented\n";
    resultValue = nullptr;
}

void LlvmIrGenerator::Visit(FunctionExpression* functionExpression)
{
    const string& funcName = functionExpression->GetName();
    Function* func = module->getFunction(funcName);
    if (func == nullptr)
    {
        resultValue = nullptr;
        cerr << "Use of undeclared function \"" << funcName << "\"\n";
        return;
    }

    if (functionExpression->GetArguments().size() != func->arg_size())
    {
        resultValue = nullptr;
        cerr << "Unexpected number of function arguments\n";
        return;
    }

    FunctionDefinition* funcDef = functions.find(funcName)->second;
    const FunctionDeclaration* funcDecl = funcDef->GetDeclaration();
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
        const TypeInfo* paramType = funcDecl->GetParameters()[i]->GetType();
        ExtendType(argType, paramType, resultValue);

        args.push_back(resultValue);
    }

    resultValue = builder.CreateCall(func, args, "call");
}

void LlvmIrGenerator::Visit(BranchExpression* branchExpression)
{
    resultValue = CreateBranch(
        branchExpression->GetIfCondition(),
        branchExpression->GetIfExpression(),
        branchExpression->GetElseExpression(),
        "if", "else", "merge", "phi"
    );
}

bool LlvmIrGenerator::Generate(SyntaxTreeNode* syntaxTree, Module*& module)
{
    module = new Module("module", context);
    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetMachine->getTargetTriple().str());

    // generate LLVM IR from syntax tree
    this->module = module;
    syntaxTree->Accept(this);

    if (resultValue == nullptr)
    {
        delete module;
        module = nullptr;
        return false;
    }
    else
    {
        return true;
    }
}

Type* LlvmIrGenerator::GetType(const TypeInfo* type)
{
    Type* llvmType = nullptr;
    if (type->IsSameAs(*TypeInfo::UnitType))
    {
        llvmType = unitType;
    }
    else if (type->IsBool())
    {
        llvmType = Type::getInt1Ty(context);
    }
    else if (type->IsInt())
    {
        llvmType = Type::getIntNTy(context, type->GetNumBits());
    }
    else
    {
        llvmType = nullptr;
    }

    return llvmType;
}

bool LlvmIrGenerator::CreateFunctionDeclaration(const SyntaxTree::FunctionDeclaration* funcDecl)
{
    // get the return type
    Type* returnType = GetType(funcDecl->GetReturnType());
    if (returnType == nullptr)
    {
        cerr << "Internal error: invalid function return type\n";
        return false;
    }

    // get the parameter types
    vector<Type*> parameters;
    parameters.reserve(funcDecl->GetParameters().size());
    for (const VariableDefinition* varDef : funcDecl->GetParameters())
    {
        Type* varType = GetType(varDef->GetType());
        parameters.push_back(varType);
    }

    FunctionType* funcType = FunctionType::get(returnType, parameters, false);
    llvm::Function::Create(funcType, Function::ExternalLinkage, funcDecl->GetName(), module);

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

    // sign extend value if needed
    if (srcType->IsInt() && dstType->IsInt() && srcType->GetNumBits() < dstType->GetNumBits())
    {
        value = CreateExt(value, dstType);
        resultType = dstType;
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
            leftValue = CreateExt(leftValue, rightType);
            resultType = rightType;
        }
        else
        {
            rightValue = CreateExt(rightValue, leftType);
            resultType = leftType;
        }
    }

    return resultType;
}

Value* LlvmIrGenerator::CreateExt(llvm::Value* value, const TypeInfo* dstType)
{
    if (dstType->IsSigned())
    {
        value = builder.CreateSExt(value, GetType(dstType), "signext");
    }
    else
    {
        value = builder.CreateZExt(value, GetType(dstType), "zeroext");
    }

    return value;
}

Value* LlvmIrGenerator::CreateBranch(Expression* conditionExpr, Expression* trueExpr, Expression* falseExpr,
                                     const char* trueName, const char* falseName, const char* mergeName, const char* phiName)
{
    // generate the condition IR
    conditionExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }
    Value* conditionValue = resultValue;

    // create the branch basic blocks
    Function* function = builder.GetInsertBlock()->getParent();
    BasicBlock* trueBlock = BasicBlock::Create(context, trueName, function);
    BasicBlock* falseBlock = BasicBlock::Create(context, falseName);
    BasicBlock* mergeBlock = BasicBlock::Create(context, mergeName);

    builder.CreateCondBr(conditionValue, trueBlock, falseBlock);

    // generate "true" block IR
    builder.SetInsertPoint(trueBlock);

    trueExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }

    // extend type if necessary
    const TypeInfo* trueType = ExtendType(trueExpr->GetType(), falseExpr->GetType(), resultValue);

    Value* ifExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "true" block
    trueBlock = builder.GetInsertBlock();

    // generate "false" block IR
    function->getBasicBlockList().push_back(falseBlock);
    builder.SetInsertPoint(falseBlock);

    falseExpr->Accept(this);
    if (resultValue == nullptr)
    {
        return nullptr;
    }

    // extend type if necessary
    const TypeInfo* falseType = ExtendType(falseExpr->GetType(), trueExpr->GetType(), resultValue);

    Value* elseExprValue = resultValue;
    builder.CreateBr(mergeBlock);

    // update block in case new blocks were added when generating the "false" block
    falseBlock = builder.GetInsertBlock();

    // generate merge block IR
    function->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

    // check if the "true" and "false" expressions have the same type
    if (!trueType->IsSameAs(*falseType))
    {
        cerr << "Internal Error: Branch true and false blocks must have the same type\n";
        return nullptr;
    }

    Type* phiType = GetType(trueType);
    PHINode* phiNode = builder.CreatePHI(phiType, 2, phiName);
    phiNode->addIncoming(ifExprValue, trueBlock);
    phiNode->addIncoming(elseExprValue, falseBlock);

    return phiNode;
}

Value* LlvmIrGenerator::CreateLogicalAnd(Expression* leftExpr, Expression* rightExpr)
{
    BoolLiteralExpression* falseExpr = BoolLiteralExpression::CreateFalseExpression();
    Value* value = CreateBranch(leftExpr, rightExpr, falseExpr, "andtrue", "andfalse", "andmerge", "andphi");
    delete falseExpr;

    return value;
}

Value* LlvmIrGenerator::CreateLogicalOr(Expression* leftExpr, Expression* rightExpr)
{
    BoolLiteralExpression* trueExpr = BoolLiteralExpression::CreateTrueExpression();
    Value* value = CreateBranch(leftExpr, trueExpr, rightExpr, "ortrue", "orfalse", "ormerge", "orphi");
    delete trueExpr;

    return value;
}
