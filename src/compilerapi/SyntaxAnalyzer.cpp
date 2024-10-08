#include "SyntaxAnalyzer.h"
#include "CompilerContext.h"
#include "keywords.h"
#include "utils.h"
#include <cassert>
#include <memory>

using namespace std;
using namespace SyntaxTree;

SyntaxAnalyzer::SyntaxAnalyzer(CompilerContext& compilerContext) :
    compilerContext(compilerContext),
    logger(compilerContext.logger)
{
}

bool SyntaxAnalyzer::Process(Modules* syntaxTree)
{
    bool ok = true;
    unsigned fileCount = compilerContext.GetFileIdCount();

    deletePointerContainer(syntaxTree->modules);
    syntaxTree->modules.reserve(fileCount);

    for (unsigned fileId = 0; fileId < fileCount; ++fileId)
    {
        ModuleDefinition* module = nullptr;
        ok = ProcessModule(fileId, compilerContext.GetFileTokens(fileId), module);
        if (!ok)
        {
            break;
        }

        syntaxTree->modules.push_back(module);
    }

    return ok;
}

bool SyntaxAnalyzer::ProcessModule(unsigned fileId, const TokenList& tokens, ModuleDefinition*& syntaxTree)
{
    TokenIterator iter = tokens.begin();
    TokenIterator endIter = tokens.end();

    vector<ConstantDeclaration*> constantDeclarations;
    vector<StructDefinition*> structs;
    vector<FunctionDefinition*> functions;
    vector<ExternFunctionDeclaration*> externFunctions;

    bool ok = true;
    while (ok && iter != endIter)
    {

        if (iter->type == Token::eConst)
        {
            ConstantDeclaration* constantDeclaration = ProcessConstantDeclaration(iter, endIter);
            if (constantDeclaration == nullptr)
            {
                ok = false;
            }
            else
            {
                constantDeclarations.push_back(constantDeclaration);
            }
        }
        else if (iter->type == Token::eFun)
        {
            FunctionDefinition* functionDefinition = ProcessFunctionDefinition(iter, endIter);
            if (functionDefinition == nullptr)
            {
                ok = false;
            }
            else
            {
                functions.push_back(functionDefinition);
            }
        }
        else if (iter->type == Token::eStruct)
        {
            StructDefinition* structDefinition = ProcessStructDefinition(iter, endIter, fileId);
            if (structDefinition == nullptr)
            {
                ok = false;
            }
            else
            {
                structs.push_back(structDefinition);
            }
        }
        else if (iter->type == Token::eExtern)
        {
            ExternFunctionDeclaration* externDecl = ProcessExternFunction(iter, endIter);
            if (externDecl == nullptr)
            {
                ok = false;
            }
            else
            {
                externFunctions.push_back(externDecl);
            }
        }
        else
        {
            ok = false;
            logger.LogError(*iter, "Unexpected token '{}'", iter->value);
        }
    }

    if (ok)
    {
        syntaxTree = new ModuleDefinition(fileId, constantDeclarations, structs, externFunctions, functions);
    }
    else
    {
        deletePointerContainer(constantDeclarations);
        deletePointerContainer(structs);
        deletePointerContainer(functions);
        deletePointerContainer(externFunctions);
        syntaxTree = nullptr;
    }

    return ok;
}

bool SyntaxAnalyzer::EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg)
{
    if (iter == endIter)
    {
        if (errorMsg == nullptr)
        {
            logger.LogError("Unexpected end of file");
        }
        else
        {
            logger.LogError("Unexpected end of file. {}", errorMsg);
        }

        return false;
    }

    return true;
}

bool SyntaxAnalyzer::IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg)
{
    ++iter;
    return EndIteratorCheck(iter, endIter, errorMsg);
}

bool SyntaxAnalyzer::IncrementIteratorCheckType(TokenIterator& iter, const TokenIterator& endIter, Token::EType expectedTokenType, const char* errorMsg)
{
    bool ok = IncrementIterator(iter, endIter, errorMsg);
    if (ok)
    {
        if (iter->type != expectedTokenType)
        {
            ok = false;
            logger.LogError(*iter, "Did not expect '{}'", iter->value);
        }
    }

    return ok;
}

ExternFunctionDeclaration* SyntaxAnalyzer::ProcessExternFunction(TokenIterator& iter,
                                                                 TokenIterator endIter)
{
    if (!EndIteratorCheck(iter, endIter, "Expected extern keyword"))
    {
        return nullptr;
    }

    if (iter->type != Token::eExtern)
    {
        logger.LogError(*iter, "Expected extern keyword");
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected function keyword"))
    {
        return nullptr;
    }

    FunctionDeclaration* decl = ProcessFunctionDeclaration(iter, endIter, Token::eSemiColon);
    if (decl == nullptr)
    {
        return nullptr;
    }

    // increment past ";"
    ++iter;

    ExternFunctionDeclaration* externFun = new ExternFunctionDeclaration(decl);
    return externFun;
}

FunctionDefinition* SyntaxAnalyzer::ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter)
{
    unique_ptr<FunctionDeclaration> functionDeclaration(ProcessFunctionDeclaration(iter, endIter, Token::eOpenBrace));
    if (functionDeclaration == nullptr)
    {
        return nullptr;
    }

    unique_ptr<Expression> expression(ProcessBlockExpression(iter, endIter));
    if (expression == nullptr)
    {
        return nullptr;
    }

    if (!EndIteratorCheck(iter, endIter))
    {
        return nullptr;
    }

    if (iter->type != Token::eCloseBrace)
    {
        logger.LogError(*iter, "Expected '}'");
        return nullptr;
    }

    // increment past "}"
    ++iter;

    FunctionDefinition* functionDefinition = new FunctionDefinition(
        functionDeclaration.release(), expression.release()
    );
    return functionDefinition;
}

FunctionDeclaration* SyntaxAnalyzer::ProcessFunctionDeclaration(TokenIterator& iter,
                                                                TokenIterator endIter,
                                                                Token::EType endTokenType)
{
    if (!EndIteratorCheck(iter, endIter, "Expected function keyword"))
    {
        return nullptr;
    }

    if (iter->type != Token::eFun)
    {
        logger.LogError(*iter, "Expected function keyword");
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected function name"))
    {
        return nullptr;
    }

    if (iter->type != Token::eIdentifier)
    {
        logger.LogError(*iter, "'{}' is not a valid function name", iter->value);
        return nullptr;
    }

    ROString functionName = iter->value;
    const Token* nameToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected '('"))
    {
        return nullptr;
    }

    if (iter->type != Token::eOpenPar)
    {
        logger.LogError(*iter, "Expected '('");
        return nullptr;
    }

    ++iter;
    Parameters parameters;
    bool ok = ProcessParameters(iter, endIter, parameters);
    if (!ok)
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected return type"))
    {
        return nullptr;
    }

    // if a return type is specified, parse it
    Expression* returnTypeExpr = nullptr;
    if (iter->type != endTokenType)
    {
        returnTypeExpr = ProcessExpression(iter, endIter, endTokenType);
        if (returnTypeExpr == nullptr)
        {
            deletePointerContainer(parameters);
            return nullptr;
        }
    }

    FunctionDeclaration* functionDeclaration = new FunctionDeclaration(
        functionName, parameters, returnTypeExpr, nameToken);
    return functionDeclaration;
}

bool SyntaxAnalyzer::ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                                       Parameters& parameters)
{
    parameters.clear();

    ROString paramName;
    const Token* paramNameToken = nullptr;
    while (iter != endIter && iter->type != Token::eClosePar)
    {
        paramName = iter->value;
        paramNameToken = &*iter;
        if (paramNameToken->type != Token::eIdentifier)
        {
            logger.LogError(*iter, "Invalid parameter name: '{}'", paramName);
            return false;
        }

        if (!IncrementIterator(iter, endIter, "Expected a type"))
        {
            return false;
        }

        Expression* paramTypeExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eClosePar);

        // make sure there was a type
        if (paramTypeExpr == nullptr)
        {
            logger.LogError(*iter, "Expected a parameter type");
            return false;
        }

        Parameter* param = new Parameter(paramName, paramTypeExpr, paramNameToken);
        parameters.push_back(param);

        if (iter->type != Token::eClosePar)
        {
            ++iter;
        }
    }

    if (iter == endIter)
    {
        logger.LogError("Expected ')'");
        return false;
    }

    return true;
}

StructDefinition* SyntaxAnalyzer::ProcessStructDefinition(TokenIterator& iter, TokenIterator endIter, unsigned fileId)
{
    if (!EndIteratorCheck(iter, endIter, "Expected struct keyword"))
    {
        return nullptr;
    }

    if (iter->type != Token::eStruct)
    {
        logger.LogError("Expected struct keyword");
    }

    if (!IncrementIterator(iter, endIter, "Expected struct name"))
    {
        return nullptr;
    }

    if (iter->type != Token::eIdentifier)
    {
        logger.LogError(*iter, "'{}' is not a valid struct name", iter->value);
        return nullptr;
    }

    const Token* structNameToken = &*iter;
    ROString structName = iter->value;

    if (!IncrementIterator(iter, endIter, "Expected '{'"))
    {
        return nullptr;
    }

    if (iter->type != Token::eOpenBrace)
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    // increment past "}"
    ++iter;

    vector<MemberDefinition*> members;
    while (iter != endIter && iter->type != Token::eCloseBrace)
    {
        // get member name
        if (iter->type != Token::eIdentifier)
        {
            deletePointerContainer(members);
            logger.LogError(*iter, "Invalid member name: '{}'", iter->value);
            return nullptr;
        }
        const Token* memberNameToken = &*iter;
        ROString memberName = iter->value;

        // get member type
        if (!IncrementIterator(iter, endIter, "Expected member type"))
        {
            deletePointerContainer(members);
            return nullptr;
        }

        Expression* memberTypeExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eCloseBrace);

        // make sure there was a type
        if (memberTypeExpr == nullptr)
        {
            deletePointerContainer(members);
            logger.LogError(*iter, "Expected a member type");
            return nullptr;
        }

        MemberDefinition* member = new MemberDefinition(memberName, memberTypeExpr, memberNameToken);
        members.push_back(member);

        Token::EType delimiter = iter->type;
        if (delimiter == Token::eCloseBrace)
        {
            break;
        }
        else if (delimiter != Token::eComma)
        {
            logger.LogError(*iter, "Expected ',' or '}'");
            deletePointerContainer(members);
            return nullptr;
        }

        ++iter;
    }

    if (iter == endIter)
    {
        deletePointerContainer(members);
        logger.LogError("Expected '}'");
        return nullptr;
    }
    else if (iter->type != Token::eCloseBrace)
    {
        deletePointerContainer(members);
        logger.LogError(*iter, "Expected '}'");
        return nullptr;
    }

    // increment past "}"
    ++iter;

    StructDefinition* structDef = new StructDefinition(structName, members, structNameToken, fileId);
    return structDef;
}

bool SyntaxAnalyzer::IsStructInitialization(TokenIterator iter, TokenIterator endIter)
{
    if (iter == endIter)
    {
        return false;
    }

    ++iter;
    if (iter == endIter || iter->type != Token::eOpenBrace)
    {
        return false;
    }

    ++iter;
    if (iter == endIter)
    {
        return false;
    }
    else if (iter->type == Token::eCloseBrace)
    {
        return true;
    }
    else if (iter->type != Token::eIdentifier)
    {
        return false;
    }

    ++iter;
    if (iter == endIter || iter->type != Token::eColon)
    {
        return false;
    }

    return true;
}

StructInitializationExpression* SyntaxAnalyzer::ProcessStructInitialization(TokenIterator& iter, TokenIterator endIter)
{
    const Token* structNameToken = &*iter;
    ROString structName = iter->value;

    // skip struct name
    ++iter;

    const Token* openBraceToken = &*iter;

    // skip '{'
    ++iter;
    if (iter == endIter)
    {
        logger.LogError("Unexpected end of file in the middle of a struct initialization");
        return nullptr;
    }

    // process member initializations
    vector<MemberInitialization*> members;
    while (iter != endIter && iter->type != Token::eCloseBrace)
    {
        // get member name
        if (iter->type != Token::eIdentifier)
        {
            deletePointerContainer(members);
            logger.LogError(*iter, "Invalid member name: '{}'", iter->value);
            return nullptr;
        }
        const Token* memberNameToken = &*iter;
        ROString memberName = iter->value;

        // make sure member name is followed by ':'
        if (!IncrementIteratorCheckType(iter, endIter, Token::eColon, "Expected ':' after member"))
        {
            deletePointerContainer(members);
            return nullptr;
        }

        // get member expression
        ++iter;
        Expression* memberExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eCloseBrace);
        if (memberExpr == nullptr)
        {
            deletePointerContainer(members);
            return nullptr;
        }

        MemberInitialization* member = new MemberInitialization(memberName, memberExpr, memberNameToken);
        members.push_back(member);

        Token::EType delimiter = iter->type;
        if (delimiter == Token::eCloseBrace)
        {
            break;
        }
        else if (delimiter != Token::eComma)
        {
            logger.LogError(*iter, "Expected ',' or '}'");
            deletePointerContainer(members);
            return nullptr;
        }

        ++iter;
    }

    if (iter == endIter)
    {
        deletePointerContainer(members);
        logger.LogError("Expected '}'");
        return nullptr;
    }
    else if (iter->type != Token::eCloseBrace)
    {
        deletePointerContainer(members);
        logger.LogError(*iter, "Expected '}'");
        return nullptr;
    }

    const Token* closeBraceToken = &*iter;

    StructInitializationExpression* structInit = new StructInitializationExpression(structName, members, structNameToken, openBraceToken, closeBraceToken);
    return structInit;
}

ConstantDeclaration* SyntaxAnalyzer::ProcessConstantDeclaration(TokenIterator& iter, TokenIterator endIter)
{
    if (iter->type != Token::eConst)
    {
        logger.LogError("Expected '{}'", CONST_KEYWORD);
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected a constant name"))
    {
        return nullptr;
    }

    if (iter->type != Token::eIdentifier)
    {
        logger.LogError(*iter, "Invalid constant name");
        return nullptr;
    }

    const Token* constNameToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected constant type or assignment operator"))
    {
        return nullptr;
    }

    Expression* typeExpr = nullptr;
    if (iter->type != Token::eEqual)
    {
        typeExpr = ProcessExpression(iter, endIter, Token::eEqual);
        if (typeExpr == nullptr)
        {
            return nullptr;
        }
    }

    const Token* opToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        delete typeExpr;
        return nullptr;
    }

    Expression* expression = ProcessExpression(iter, endIter, Token::eSemiColon, Token::eCloseBrace);
    if (expression == nullptr)
    {
        delete typeExpr;
        return nullptr;
    }

    // ensure iter is a semicolon
    if (iter == endIter)
    {
        logger.LogError("Unexpected end of file");
        delete typeExpr;
        return nullptr;
    }

    if (iter->type != Token::eSemiColon)
    {
        logger.LogError(*iter, "Expected ';'");
        delete typeExpr;
        return nullptr;
    }

    // increment past semicolon
    ++iter;

    ROString constName = constNameToken->value;
    BinaryExpression* assignment = new BinaryExpression(BinaryExpression::eAssign, new IdentifierExpression(constName, constNameToken), expression, opToken);
    ConstantDeclaration* constDecl = new ConstantDeclaration(constName, assignment, typeExpr, constNameToken);
    return constDecl;
}

VariableDeclaration* SyntaxAnalyzer::ProcessVariableDeclaration(TokenIterator& iter, TokenIterator endIter)
{
    if (iter->type != Token::eVar)
    {
        logger.LogError("Expected '{}'", VARIABLE_KEYWORD);
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected a variable name"))
    {
        return nullptr;
    }

    if (iter->type != Token::eIdentifier)
    {
        logger.LogError(*iter, "Invalid variable name");
        return nullptr;
    }

    const Token* varNameToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected variable type or assignment operator"))
    {
        return nullptr;
    }

    Expression* typeExpr = nullptr;
    if (iter->type != Token::eEqual)
    {
        typeExpr = ProcessExpression(iter, endIter, Token::eEqual);
        if (typeExpr == nullptr)
        {
            return nullptr;
        }
    }

    const Token* opToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        delete typeExpr;
        return nullptr;
    }

    Expression* expression = ProcessExpression(iter, endIter, Token::eSemiColon, Token::eCloseBrace);
    if (expression == nullptr)
    {
        delete typeExpr;
        return nullptr;
    }

    // ensure iter is a semicolon
    if (iter == endIter)
    {
        logger.LogError("Unexpected end of file");
        delete typeExpr;
        return nullptr;
    }

    if (iter->type != Token::eSemiColon)
    {
        logger.LogError(*iter, "Expected ';'");
        delete typeExpr;
        return nullptr;
    }

    // increment past semicolon
    ++iter;

    ROString varName = varNameToken->value;
    BinaryExpression* assignment = new BinaryExpression(BinaryExpression::eAssign, new IdentifierExpression(varName, varNameToken), expression, opToken);
    VariableDeclaration* varDecl = new VariableDeclaration(varName, assignment, typeExpr, varNameToken);
    return varDecl;
}

WhileLoop* SyntaxAnalyzer::ProcessWhileLoop(TokenIterator& iter, TokenIterator endIter)
{
    assert(iter->type == Token::eWhile);

    const Token* whileToken = &*iter;

    // increment iter past "while" keyword
    ++iter;

    // read "while" condition
    unique_ptr<Expression> whileCondition(ProcessExpression(iter, endIter, Token::eOpenBrace));
    if (whileCondition == nullptr)
    {
        return nullptr;
    }

    if (iter == endIter || iter->type != Token::eOpenBrace)
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    unique_ptr<BlockExpression> expression(ProcessBlockExpression(iter, endIter));
    if (expression == nullptr)
    {
        return nullptr;
    }

    // increment iter past end brace
    ++iter;

    WhileLoop* whileLoop = new WhileLoop(whileCondition.release(), expression.release(), whileToken);
    return whileLoop;
}

ForLoop* SyntaxAnalyzer::ProcessForLoop(TokenIterator& iter, TokenIterator endIter)
{
    assert(iter->type == Token::eFor);

    const Token* forToken = &*iter;

    // increment iter past "for" keyword
    ++iter;

    // read variable name
    if (!EndIteratorCheck(iter, endIter, "Expected a variable name"))
    {
        return nullptr;
    }

    if (iter->type != Token::eIdentifier)
    {
        logger.LogError(*iter, "Invalid iterator variable name");
        return nullptr;
    }

    const Token* varNameToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected variable type, ',', or 'in' keyword"))
    {
        return nullptr;
    }

    Expression* varTypeExpr = nullptr;
    if (iter->type != Token::eComma && iter->type != Token::eIn)
    {
        varTypeExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eIn);
        if (varTypeExpr == nullptr)
        {
            return nullptr;
        }
    }

    // check if there's an index variable
    const Token* indexVarNameToken = Token::None;
    Expression* indexVarTypeExpr = nullptr;
    if (iter->type == Token::eComma)
    {
        // read variable name
        if (!IncrementIterator(iter, endIter, "Expected variable name"))
        {
            return nullptr;
        }

        if (iter->type != Token::eIdentifier)
        {
            logger.LogError(*iter, "Invalid index variable name");
            return nullptr;
        }

        indexVarNameToken = &*iter;

        if (!IncrementIterator(iter, endIter, "Expected variable type or 'in' keyword"))
        {
            return nullptr;
        }

        if (iter->type != Token::eIn)
        {
            indexVarTypeExpr = ProcessExpression(iter, endIter, Token::eIn);
            if (indexVarTypeExpr == nullptr)
            {
                return nullptr;
            }
        }
    }

    const Token* inToken = &*iter;

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        return nullptr;
    }

    // read "for" iterable expression
    unique_ptr<Expression> iterExpression(ProcessExpression(iter, endIter, Token::eOpenBrace));
    if (iterExpression == nullptr)
    {
        return nullptr;
    }

    if (iter == endIter || iter->type != Token::eOpenBrace)
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    unique_ptr<BlockExpression> expression(ProcessBlockExpression(iter, endIter));
    if (expression == nullptr)
    {
        return nullptr;
    }

    // increment iter past end brace
    ++iter;

    ForLoop* forLoop = new ForLoop(varNameToken->value, varTypeExpr,
                                   indexVarNameToken->value, indexVarTypeExpr,
                                   iterExpression.release(), expression.release(),
                                   forToken, inToken,
                                   varNameToken, indexVarNameToken);
    return forLoop;
}

SyntaxAnalyzer::TokenIterator SyntaxAnalyzer::FindStatementEnd(TokenIterator iter, TokenIterator endIter)
{
    unsigned int balance = 0;

    while (iter != endIter)
    {
        Token::EType type = iter->type;
        if (balance == 0)
        {
            if (type == Token::eSemiColon || type == Token::eCloseBrace)
            {
                break;
            }
        }
        else if (type == Token::eOpenBrace)
        {
            ++balance;
        }
        else if (type == Token::eCloseBrace)
        {
            --balance;
        }

        ++iter;
    }

    return iter;
}

Expression* SyntaxAnalyzer::AddUnaryExpressions(Expression* baseExpr, stack<UnaryOpData>& unaryOperators)
{
    Expression* result = baseExpr;
    while (!unaryOperators.empty())
    {
        UnaryOpData data = unaryOperators.top();
        result = new UnaryExpression(data.op, result, data.token);
        unaryOperators.pop();
    }

    return result;
}

Expression* SyntaxAnalyzer::ProcessTerm(
    TokenIterator& iter,
    TokenIterator endIter,
    bool& isPotentialEnd,
    Token::EType endTokenType1,
    Token::EType endTokenType2,
    Token::EType endTokenType3)
{
    Token::EType type = iter->type;
    Token::EMainType mainType = Token::GetMainType(type);
    Expression* expr = nullptr;

    if (mainType == Token::eIntLiteralType)
    {
        int64_t intValue = 0;
        stringToInteger(iter->value, intValue);
        expr = new NumericExpression(intValue, &*iter);
    }
    else if (mainType == Token::eFloatLiteralType)
    {
        double floatValue = stringToFloat(iter->value);
        expr = new FloatLiteralExpression(floatValue, &*iter);
    }
    else if (mainType == Token::eBoolLiteralType)
    {
        expr = new BoolLiteralExpression(&*iter);
    }
    else if (mainType == Token::eStrLiteralType)
    {
        expr = ProcessStringExpression(iter);
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (type == Token::eOpenPar)
    {
        TokenIterator parenEndIter = FindParenthesisEnd(iter, endIter);
        if (parenEndIter == endIter)
        {
            logger.LogError(*iter, "Could not find end parenthesis");
            return nullptr;
        }

        ++iter;
        expr = ProcessExpression(iter, parenEndIter);
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (type == Token::eOpenBrace)
    {
        expr = ProcessBlockExpression(iter, endIter);
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (type == Token::eIf)
    {
        expr = ProcessBranchExpression(iter, endIter);
        if (expr == nullptr)
        {
            return nullptr;
        }

        isPotentialEnd = true;
    }
    else if (type == Token::eIdentifier)
    {
        // check if it's a struct initialization
        if (IsStructInitialization(iter, endIter))
        {
            expr = ProcessStructInitialization(iter, endIter);
            if (expr == nullptr)
            {
                return nullptr;
            }
        }
        else // it's an identifier
        {
            expr = new IdentifierExpression(iter->value, &*iter);
        }
    }
    else if (Token::IsTypeName(type))
    {
        expr = new IdentifierExpression(iter->value, &*iter);
    }
    else if (type == Token::eOpenBracket)
    {
        const Token* startToken = &*iter;

        if (!IncrementIterator(iter, endIter))
        {
            return nullptr;
        }

        if (iter->type == Token::eCloseBracket)
        {
            logger.LogError(*iter, "Array expressions cannot be empty");
            return nullptr;
        }

        Expression* expr1 = ProcessExpression(iter, endIter, Token::eSemiColon, Token::eComma, Token::eCloseBracket);
        if (expr1 == nullptr)
        {
            return nullptr;
        }

        if (iter->type == Token::eSemiColon)
        {
            if (!IncrementIterator(iter, endIter, "Expected expression"))
            {
                return nullptr;
            }

            Expression* expr2 = ProcessExpression(iter, endIter, Token::eCloseBracket);
            if (expr2 == nullptr)
            {
                return nullptr;
            }

            expr = new ArraySizeValueExpression(expr1, expr2, startToken, &*iter);
        }
        else if (iter->type == Token::eComma || iter->type == Token::eCloseBracket)
        {
            vector<Expression*> expressions;
            expressions.push_back(expr1);

            while (iter->type == Token::eComma)
            {
                if (!IncrementIterator(iter, endIter))
                {
                    deletePointerContainer(expressions);
                    return nullptr;
                }

                // if there is a ']' immediately after the ',' then we are done
                if (iter->type == Token::eCloseBracket)
                {
                    break;
                }

                Expression* expr2 = ProcessExpression(iter, endIter, Token::eComma, Token::eCloseBracket);
                if (expr2 == nullptr)
                {
                    deletePointerContainer(expressions);
                    return nullptr;
                }

                expressions.push_back(expr2);
            }

            expr = new ArrayMultiValueExpression(expressions, startToken, &*iter);
        }
        else
        {
            logger.LogInternalError("Unexpected separating character after first array element");
            return nullptr;
        }
    }
    else if (type == Token::eCast)
    {
        const Token* castToken = &*iter;

        // ensure there's an open parenthesis after the cast
        ++iter;
        if (iter == endIter || iter->type != Token::eOpenPar)
        {
            logger.LogError(*castToken, "Expected open parenthesis after cast");
            return nullptr;
        }

        const Token* openParToken = &*iter;

        ++iter;
        if (iter == endIter)
        {
            logger.LogError("Unexpected end of file in the middle of a cast");
            return nullptr;
        }

        // parse the cast type name
        Expression* typeExpr = ProcessExpression(iter, endIter, Token::eComma);

        // make sure there was a type
        if (typeExpr == nullptr)
        {
            logger.LogError(*iter, "Expected a cast type");
            return nullptr;
        }

        ++iter;
        if (iter == endIter)
        {
            delete typeExpr;
            logger.LogError("Unexpected end of file in the middle of a cast");
            return nullptr;
        }

        // parse the expression
        Expression* subExpression = ProcessExpression(iter, endIter, Token::eComma, Token::eClosePar);
        if (subExpression == nullptr)
        {
            delete typeExpr;
            return nullptr;
        }

        if (iter->type == Token::eComma)
        {
            ++iter;
            if (iter == endIter)
            {
                delete typeExpr;
                delete subExpression;
                logger.LogError("Unexpected end of file in the middle of a cast");
                return nullptr;
            }
        }

        if (iter->type != Token::eClosePar)
        {
            delete typeExpr;
            delete subExpression;
            logger.LogError(*castToken, "Cast does not have closing parenthesis");
            return nullptr;
        }

        const Token* closeParToken = &*iter;

        expr = new CastExpression(typeExpr, subExpression, castToken, openParToken, closeParToken);
    }
    else if (type == Token::eFun)
    {
        const Token* funToken = &*iter;
        if (!IncrementIterator(iter, endIter, "Unexpected end of file"))
        {
            return nullptr;
        }
        if (iter->type != Token::eOpenPar)
        {
            logger.LogError(*iter, "Expected '('");
            return nullptr;
        }

        const Token* openParToken = &*iter;

        // parse parameters
        vector<ROString> paramNames;
        vector<const Token*> paramNameTokens;
        vector<Expression*> paramTypes;

        // increment past '('
        if (!IncrementIterator(iter, endIter, "Unexpected end of file"))
        {
            return nullptr;
        }

        while (iter != endIter && iter->type != Token::eClosePar)
        {
            if (iter->type != Token::eIdentifier)
            {
                logger.LogError(*iter, "Invalid parameter name: '{}'", iter->value);
                return nullptr;
            }

            paramNames.push_back(iter->value);
            paramNameTokens.push_back(&*iter);

            if (!IncrementIterator(iter, endIter, "Expected a type"))
            {
                return nullptr;
            }

            Expression* paramTypeExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eClosePar);
            if (paramTypeExpr == nullptr)
            {
                return nullptr;
            }

            paramTypes.push_back(paramTypeExpr);

            if (iter->type != Token::eClosePar)
            {
                ++iter;
            }
        }

        if (iter == endIter)
        {
            logger.LogError("Expected ')'");
            return nullptr;
        }

        const Token* closeParToken = &*iter;

        // parse return type
        Expression* returnType = nullptr;
        TokenIterator nextIter = iter + 1;
        if (nextIter != endIter
        && nextIter->type != Token::eCloseBracket // TODO: fix this 'cause it's a bit hacky
        && nextIter->type != endTokenType1
        && nextIter->type != endTokenType2
        && nextIter->type != endTokenType3)
        {
            ++iter;
            returnType = ProcessExpression(iter, endIter, endTokenType1, endTokenType2, endTokenType3);
            if (returnType == nullptr)
            {
                return nullptr;
            }

            if (iter != endIter)
            {
                Token::EType tokenType = iter->type;
                if (tokenType == endTokenType1 || tokenType == endTokenType2 || tokenType == endTokenType3)
                {
                    // need to move back to the previous token because it will be incremented
                    // after processing this term
                    --iter;
                }
            }
        }

        expr = new FunctionTypeExpression(paramTypes, paramNames, returnType, funToken, openParToken, closeParToken, paramNameTokens);
    }
    else
    {
        logger.LogError(*iter, "Unexpected term '{}'", iter->value);
        return nullptr;
    }

    return expr;
}

const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS1 = {BinaryExpression::eMultiply, BinaryExpression::eDivide, BinaryExpression::eRemainder};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS2 = {BinaryExpression::eAdd, BinaryExpression::eSubtract};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS3 = {BinaryExpression::eShiftLeft, BinaryExpression::eShiftRightLogical, BinaryExpression::eShiftRightArithmetic};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS4 = {BinaryExpression::eBitwiseAnd};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS5 = {BinaryExpression::eBitwiseXor};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS6 = {BinaryExpression::eBitwiseOr};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS7 = {BinaryExpression::eClosedRange, BinaryExpression::eHalfOpenRange};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS8 = {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS9 = {BinaryExpression::eLogicalAnd, BinaryExpression::eLogicalOr};
const unordered_set<BinaryExpression::EOperator> BIN_OPERATORS10 = {
            BinaryExpression::eAssign,
            BinaryExpression::eAddAssign,
            BinaryExpression::eSubtractAssign,
            BinaryExpression::eMultiplyAssign,
            BinaryExpression::eDivideAssign,
            BinaryExpression::eRemainderAssign,
            BinaryExpression::eShiftLeftAssign,
            BinaryExpression::eShiftRightLogicalAssign,
            BinaryExpression::eShiftRightArithmeticAssign,
            BinaryExpression::eBitwiseAndAssign,
            BinaryExpression::eBitwiseXorAssign,
            BinaryExpression::eBitwiseOrAssign,
};

Expression* SyntaxAnalyzer::ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              Token::EType endTokenType1,
                                              Token::EType endTokenType2,
                                              Token::EType endTokenType3)
{
    bool expectTerm = true;
    bool isEnd = false;
    bool isPotentialEnd = false;
    vector<Expression*> terms;
    stack<UnaryOpData> unaryOperators;
    vector<BinaryOpData> binOperators;

    while (iter != endIter)
    {
        Token::EType tokenType = iter->type;
        TokenIterator nextIter = iter + 1;

        if (tokenType == endTokenType1 || tokenType == endTokenType2 || tokenType == endTokenType3)
        {
            break;
        }

        isEnd = false;
        if (expectTerm)
        {
            isPotentialEnd = false;

            if (Token::IsUnaryOp(tokenType))
            {
                unaryOperators.push({&*iter, static_cast<UnaryExpression::EOperator>(tokenType)});
                expectTerm = true;
            }
            // the lexer will give us two characters together as one token
            else if (tokenType == Token::eAmpersandAmpersand)
            {
                // create two tokens from one
                const Token* originalToken = &*iter;

                // TODO: Memory leak. Someday, I should fix this...
                Token* token1 =
                    new Token(POINTER_TYPE_TOKEN,
                            originalToken->filenameId,
                            originalToken->line,
                            originalToken->column,
                            Token::eAmpersand);
                Token* token2 =
                    new Token(POINTER_TYPE_TOKEN,
                            originalToken->filenameId,
                            originalToken->line,
                            originalToken->column + 1,
                            Token::eAmpersand);

                unaryOperators.push({token1, UnaryExpression::eAddressOf});
                unaryOperators.push({token2, UnaryExpression::eAddressOf});
                expectTerm = true;
            }
            else if (tokenType == Token::eOpenBracket && nextIter != endIter && nextIter->type == Token::eCloseBracket)
            {
                unaryOperators.push({&*iter, UnaryExpression::eArrayOf});
                expectTerm = true;
                ++iter;
            }
            else
            {
                Expression* expr = ProcessTerm(iter, endIter, isPotentialEnd, endTokenType1, endTokenType2, endTokenType3);
                if (expr == nullptr)
                {
                    deletePointerContainer(terms);
                    return nullptr;
                }

                // update nextIter since iter may have changed in ProcessTerm()
                nextIter = (iter == endIter) ? endIter : iter + 1;

                if ( nextIter != endIter && (nextIter->type == Token::ePeriod || nextIter->type == Token::eOpenPar || nextIter->type == Token::eOpenBracket) )
                {
                    expr = ProcessPostTerm(expr, iter, endIter);
                    if (expr == nullptr)
                    {
                        deletePointerContainer(terms);
                        return nullptr;
                    }
                }

                expr = AddUnaryExpressions(expr, unaryOperators);
                terms.push_back(expr);

                expectTerm = false;
            }
        }
        else
        {
            // if the token is a binary operator, add it to the list
            if (Token::IsBinaryOp(tokenType))
            {
                binOperators.push_back({&*iter, static_cast<BinaryExpression::EOperator>(tokenType)});
                expectTerm = true;
            }
            // if we are at the end of an expression, we're done
            else if (isPotentialEnd)
            {
                break;
            }
            else
            {
                logger.LogError(*iter, "Expected an operator, but got '{}' instead", iter->value);
                deletePointerContainer(terms);
                return nullptr;
            }
        }

        if (isEnd)
        {
            break;
        }

        ++iter;
    }

    if (expectTerm)
    {
        if (iter == endIter)
        {
            logger.LogError("Expected another expression term");
        }
        else
        {
            logger.LogError(*iter, "Expected another expression term");
        }

        deletePointerContainer(terms);
        return nullptr;
    }

    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS1);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS2);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS3);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS4);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS5);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS6);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS7);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS8);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS9);
    ProcessExpressionOperators(terms, binOperators, BIN_OPERATORS10);

    return terms.front();
}

SyntaxAnalyzer::TokenIterator SyntaxAnalyzer::FindParenthesisEnd(TokenIterator iter, TokenIterator endIter)
{
    unsigned int balance = 1;

    ++iter;
    while (iter != endIter)
    {
        if (iter->type == Token::eOpenPar)
        {
            ++balance;
        }
        else if (iter->type == Token::eClosePar)
        {
            --balance;
            if (balance == 0)
            {
                return iter;
            }
        }

        ++iter;
    }

    return endIter;
}

void SyntaxAnalyzer::ProcessExpressionOperators(vector<Expression*>& terms,
                                                vector<BinaryOpData>& operators,
                                                const unordered_set<BinaryExpression::EOperator>& opsToProcess)
{
    if (terms.size() != operators.size() + 1)
    {
        logger.LogInternalError("Expression terms and operators do not match");
        return;
    }

    auto term1Iter = terms.begin();
    auto opIter = operators.begin();
    while (opIter != operators.end())
    {
        if (opsToProcess.find(opIter->op) != opsToProcess.cend())
        {
            auto term2Iter = term1Iter + 1;
            *term1Iter = new BinaryExpression(opIter->op, *term1Iter, *term2Iter, opIter->token);

            terms.erase(term2Iter);
            opIter = operators.erase(opIter);
        }
        else
        {
            ++term1Iter;
            ++opIter;
        }
    }
}

StringLiteralExpression* SyntaxAnalyzer::ProcessStringExpression(TokenIterator iter)
{
    ROString value = iter->value;
    const char* valuePtr = value.GetPtr();
    size_t valueSize = value.GetSize();

    size_t idx = 0;
    if (valueSize < 1 || valuePtr[idx] != '"')
    {
        logger.LogError(*iter, "String doesn't start with '\"'");
        return nullptr;
    }

    vector<char> chars;
    chars.reserve(valueSize);

    ++idx;
    size_t endCharsIdx = valueSize - 1;
    while (idx < endCharsIdx)
    {
        unsigned continuingByteCount = 0;

        char ch = valuePtr[idx];
        if (ch == '\\')
        {
            // make sure we're not at the end of the string
            if (idx >= endCharsIdx - 1)
            {
                logger.LogError(*iter, "Start of escape sequence at end of string");
                return nullptr;
            }

            ++idx;
            ch = valuePtr[idx];

            if (ch == '\\')
            {
                chars.push_back('\\');
            }
            else if (ch == '\'')
            {
                chars.push_back('\'');
            }
            else if (ch == '"')
            {
                chars.push_back('"');
            }
            else if (ch == 'n')
            {
                chars.push_back('\n');
            }
            else if (ch == 'r')
            {
                chars.push_back('\r');
            }
            else if (ch == 't')
            {
                chars.push_back('\t');
            }
            else if (ch == 'x')
            {
                bool ok = ProcessByteEscapeSequence(iter, idx, chars);
                if (!ok)
                {
                    return nullptr;
                }
            }
            else if (ch == 'u')
            {
                bool ok = ProcessUnicodeEscapeSequence(iter, idx, chars);
                if (!ok)
                {
                    return nullptr;
                }
            }
            else
            {
                logger.LogError(*iter, "Invalid escape sequence '\\{}'", ch);
                return nullptr;
            }
        }
        else if (is1ByteUtf8(ch)) // 1-byte UTF-8 sequence
        {
            chars.push_back(ch);
            continuingByteCount = 0;
        }
        else if (is2ByteUtf8Start(ch)) // 2-byte UTF-8 sequence
        {
            chars.push_back(ch);
            continuingByteCount = 1;
        }
        else if (is3ByteUtf8Start(ch)) // 3-byte UTF-8 sequence
        {
            chars.push_back(ch);
            continuingByteCount = 2;
        }
        else if (is4ByteUtf8Start(ch)) // 4-byte UTF-8 sequence
        {
            chars.push_back(ch);
            continuingByteCount = 3;
        }
        else
        {
            logger.LogError(*iter, "Invalid character in string");
            return nullptr;
        }

        // check and add any continuing bytes in a multi-byte UTF-8 sequence
        for (unsigned i = 0; i < continuingByteCount; ++i)
        {
            // make sure we're not at the end of the string
            if (idx >= endCharsIdx - 1)
            {
                logger.LogError(*iter, "Start of multi-byte UTF-8 sequence at end of string");
                return nullptr;
            }

            ++idx;
            ch = valuePtr[idx];

            // make sure the continuing byte is valid
            if (!isUtf8Continuation(ch))
            {
                logger.LogError(*iter, "Invalid continuing byte of multi-byte UTF-8 sequence");
                return nullptr;
            }

            chars.push_back(ch);
        }

        ++idx;
    }

    if (valuePtr[valueSize - 1] != '"')
    {
        logger.LogError(*iter, "String doesn't end with '\"'");
        return nullptr;
    }

    StringLiteralExpression* expr = new StringLiteralExpression(chars, &*iter);
    return expr;
}

bool SyntaxAnalyzer::ProcessByteEscapeSequence(const TokenIterator& iter, size_t& idx, vector<char>& chars)
{
    ROString value = iter->value;
    size_t endCharsIdx = value.GetSize() - 1;

    char ch = '\0';
    uint8_t byte = 0;
    for (size_t i = 0; i < 2; ++i)
    {
        byte <<= 4;

        ++idx;
        if (idx >= endCharsIdx)
        {
            logger.LogError(*iter, "Reached end of string before end of '\\x' escape sequence");
            return false;
        }

        ch = value[idx];

        char digitNum = '\0';
        if (hexDigitToNum(ch, digitNum))
        {
            byte |= digitNum;
        }
        else
        {
            logger.LogError(*iter, "Invalid hexadecimal digit in '\\x' escape sequence");
            return false;
        }
    }

    if (!is1ByteUtf8(byte))
    {
        logger.LogError(*iter, "Invalid UTF-8 byte in '\\x' escape sequence");
        return false;
    }

    chars.push_back(byte);
    return true;
}

bool SyntaxAnalyzer::ProcessUnicodeEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars)
{
    ROString value = iter->value;
    size_t endCharsIdx = value.GetSize() - 1;

    ++idx;
    if (idx >= endCharsIdx)
    {
        logger.LogError(*iter, "Reached end of string before end of '\\u' escape sequence");
        return false;
    }

    char ch = value[idx];
    if (ch != '{')
    {
        logger.LogError(*iter, "Expected '{' after '\\u'");
        return false;
    }

    ++idx;
    if (idx >= endCharsIdx)
    {
        logger.LogError(*iter, "Reached end of string before end of '\\u' escape sequence");
        return false;
    }

    ch = value[idx];
    if (ch == '}')
    {
        logger.LogError(*iter, "Unexpected '}' after '{'");
        return false;
    }

    unsigned digitCount = 0;
    uint32_t codePoint = 0;
    while (ch != '}')
    {
        ++digitCount;
        if (digitCount > 8)
        {
            logger.LogError(*iter, "'\\u' escape sequence cannot contain more than 8 digits");
            return false;
        }

        codePoint <<= 4;

        char digitNum = '\0';
        if (hexDigitToNum(ch, digitNum))
        {
            codePoint |= digitNum;
        }
        else
        {
            logger.LogError(*iter, "Invalid hexadecimal digit in '\\u' escape sequence");
            return false;
        }

        ++idx;
        if (idx >= endCharsIdx)
        {
            logger.LogError(*iter, "Reached end of string before end of '\\u' escape sequence");
            return false;
        }

        ch = value[idx];
    }

    // check if we can encode in a 1-byte UTF-8 sequence
    if (codePoint <= 0x7f)
    {
        uint8_t byte = codePoint & 0x007f;
        chars.push_back(byte);
    }
    // check if we can encode in a 2-byte UTF-8 sequence
    else if (codePoint <= 0x07ff)
    {
        uint8_t byte = (codePoint & 0x07c0) >> 6;
        byte |= 0xc0;
        chars.push_back(byte);

        byte = codePoint & 0x003f;
        byte |= 0x80;
        chars.push_back(byte);
    }
    // check if we can encode in a 3-byte UTF-8 sequence
    else if (codePoint <= 0xffff)
    {
        uint8_t byte = (codePoint & 0xf000) >> 12;
        byte |= 0xe0;
        chars.push_back(byte);

        byte = (codePoint & 0x0fc0) >> 6;
        byte |= 0x80;
        chars.push_back(byte);

        byte = codePoint & 0x003f;
        byte |= 0x80;
        chars.push_back(byte);
    }
    // check if we can encode in a 4-byte UTF-8 sequence
    else if (codePoint <= 0x10'ffff)
    {
        uint8_t byte = (codePoint & 0x1c'0000) >> 18;
        byte |= 0xf0;
        chars.push_back(byte);

        byte = (codePoint & 0x03'f000) >> 12;
        byte |= 0x80;
        chars.push_back(byte);

        byte = (codePoint & 0x00'0fc0) >> 6;
        byte |= 0x80;
        chars.push_back(byte);

        byte = codePoint & 0x00'003f;
        byte |= 0x80;
        chars.push_back(byte);
    }
    else
    {
        logger.LogError(*iter, "Unicode sequence exceeds max value");
        return false;
    }

    return true;
}

BlockExpression* SyntaxAnalyzer::ProcessBlockExpression(TokenIterator& iter, TokenIterator endIter)
{
    bool needsUnitType = true;
    SyntaxTreeNodes statements;

    const Token* startToken = &*iter;

    // increment iter past "{"
    ++iter;

    while (iter != endIter && iter->type != Token::eCloseBrace)
    {
        SyntaxTreeNode* statement = nullptr;

        Token::EType tokenType = iter->type;
        if (tokenType == Token::eVar)
        {
            statement = ProcessVariableDeclaration(iter, endIter);
            needsUnitType = true;
        }
        else if (tokenType == Token::eConst)
        {
            statement = ProcessConstantDeclaration(iter, endIter);
            needsUnitType = true;
        }
        else if (tokenType == Token::eIf)
        {
            BranchExpression* branchExpr = ProcessBranchExpression(iter, endIter);
            statement = branchExpr;

            if (branchExpr != nullptr)
            {
                ++iter; // increment past end brace

                // we need to add a unit type if there is not an else expression
                needsUnitType = dynamic_cast<UnitTypeLiteralExpression*>(branchExpr->elseExpression) != nullptr;
            }
        }
        else if (tokenType == Token::eOpenBrace)
        {
            BlockExpression* blockExpr = ProcessBlockExpression(iter, endIter);
            statement = blockExpr;

            ++iter; // increment past end brace
            needsUnitType = false;
        }
        else if (tokenType  == Token::eWhile)
        {
            statement = ProcessWhileLoop(iter, endIter);
            needsUnitType = true;
        }
        else if (tokenType == Token::eFor)
        {
            statement = ProcessForLoop(iter, endIter);
            needsUnitType = true;
        }
        else if (tokenType == Token::eBreak || tokenType == Token::eContinue)
        {
            TokenIterator tokenIter = iter;
            ++iter;

            if (iter == endIter || iter->type != Token::eSemiColon)
            {
                logger.LogError(*tokenIter, "Expected ';' after '{}'", tokenIter->value);
                statement = nullptr;
            }
            else
            {
                statement = new LoopControl(&*tokenIter);

                ++iter;
            }
            needsUnitType = true;
        }
        else if (tokenType == Token::eReturn)
        {
            const Token* token = &*iter;
            if (IncrementIterator(iter, endIter))
            {
                Expression* returnExpression = nullptr;
                if (iter->type == Token::eSemiColon)
                {
                    returnExpression = new UnitTypeLiteralExpression();
                }
                else
                {
                    returnExpression = ProcessExpression(iter, endIter, Token::eSemiColon);
                }

                statement = new Return(token, returnExpression);
                ++iter;
                needsUnitType = false;
            }
            else
            {
                statement = nullptr;
            }
        }
        else
        {
            // process the sub-expression
            statement = ProcessExpression(iter, endIter, Token::eSemiColon, Token::eCloseBrace);
            if (statement != nullptr && iter != endIter)
            {
                // if we reached the end of a statement, increment the iterator
                if (iter->type == Token::eSemiColon)
                {
                    ++iter;
                    needsUnitType = true;
                }
                // if we reached the end of a block, we're done, and the last expression is the
                // block's return type (so we don't need the unit type expression)
                else if (iter->type == Token::eCloseBrace)
                {
                    needsUnitType = false;
                }
            }
        }

        // if there was an error, return null
        if (statement == nullptr)
        {
            deletePointerContainer(statements);
            return nullptr;
        }

        statements.push_back(statement);

        // if we reached the end, log an error and return null
        if (!EndIteratorCheck(iter, endIter, "Expected block end"))
        {
            deletePointerContainer(statements);
            return nullptr;
        }
    }

    if (!EndIteratorCheck(iter, endIter, "Expected block end"))
    {
        deletePointerContainer(statements);
        return nullptr;
    }
    const Token* endToken = &*iter;

    if (needsUnitType)
    {
        statements.push_back(new UnitTypeLiteralExpression());
    }

    BlockExpression* blockExpression = new BlockExpression(statements, startToken, endToken);
    return blockExpression;
}

BranchExpression* SyntaxAnalyzer::ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter)
{
    const Token* ifToken = &*iter;

    // increment iter past "if" or "elif" keyword
    ++iter;

    // read "if" condition
    unique_ptr<Expression> ifCondition(ProcessExpression(iter, endIter, Token::eOpenBrace));
    if (ifCondition == nullptr)
    {
        return nullptr;
    }

    if (iter == endIter || iter->type != Token::eOpenBrace)
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    // read "if" expression
    unique_ptr<Expression> ifExpression(ProcessBlockExpression(iter, endIter));
    if (ifExpression == nullptr)
    {
        return nullptr;
    }

    Token::EType nextTokenType = Token::eInvalid;
    if (iter != endIter)
    {
        auto nextIter = iter + 1;
        if (nextIter != endIter)
        {
            nextTokenType = nextIter->type;
        }
    }

    const Token* elseToken = nullptr;
    unique_ptr<Expression> elseExpression;
    if (nextTokenType == Token::eElif)
    {
        // move to 'elif' keyword
        ++iter;

        elseToken = &*iter;

        // parse "elif"
        elseExpression.reset(ProcessBranchExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }
    }
    else if (nextTokenType == Token::eElse)
    {
        // move to 'else' keyword
        ++iter;

        elseToken = &*iter;

        if (!IncrementIterator(iter, endIter, "Expected '{'"))
        {
            return nullptr;
        }

        if (iter->type != Token::eOpenBrace)
        {
            logger.LogError(*iter, "Expected '{'");
            return nullptr;
        }

        // read "else" expression
        elseExpression.reset(ProcessBlockExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        elseToken = Token::None;

        // if there is no 'elif' or 'else' clause, set the else expression
        // to a unit type literal
        elseExpression.reset(new UnitTypeLiteralExpression());
    }

    BranchExpression* expr = new BranchExpression(ifCondition.release(),
                                                  ifExpression.release(),
                                                  elseExpression.release(),
                                                  ifToken, elseToken);
    return expr;
}

Expression* SyntaxAnalyzer::ProcessPostTerm(Expression* expr, TokenIterator& iter, TokenIterator endIter)
{
    TokenIterator nextIter = iter + 1;

    while ( nextIter != endIter && (nextIter->type == Token::ePeriod || nextIter->type == Token::eOpenPar || nextIter->type == Token::eOpenBracket) )
    {
        // process member expressions
        while (nextIter != endIter && nextIter->type == Token::ePeriod)
        {
            const Token* opToken = &*nextIter;

            // skip to token after "."
            iter += 2;

            if (iter == endIter)
            {
                logger.LogError("No member name after member operator");
                delete expr;
                return nullptr;
            }
            else if (iter->type != Token::eIdentifier)
            {
                logger.LogError(*iter, "Invalid member name");
                delete expr;
                return nullptr;
            }

            expr = new MemberExpression(expr, iter->value, opToken, &*iter);

            nextIter = iter + 1;
        }

        // process function calls
        while (nextIter != endIter && nextIter->type == Token::eOpenPar)
        {
            const Token* openParToken = &*nextIter;

            // skip to token after "("
            iter += 2;
            if (iter == endIter)
            {
                logger.LogError("Unexpected end of file in the middle of a function call");
                return nullptr;
            }

            // process arguments
            vector<Expression*> arguments;
            while (iter->type != Token::eClosePar)
            {
                Expression* argExpr = ProcessExpression(iter, endIter, Token::eComma, Token::eClosePar);
                if (argExpr == nullptr)
                {
                    deletePointerContainer(arguments);
                    return nullptr;
                }
                arguments.push_back(argExpr);

                if (iter->type == Token::eComma)
                {
                    ++iter;
                }
            }

            const Token* closeParToken = &*iter;

            expr = new FunctionCallExpression(expr, arguments, openParToken, closeParToken);

            nextIter = iter + 1;
        }

        // process subscript expressions
        while (nextIter != endIter && nextIter->type == Token::eOpenBracket)
        {
            const Token* opToken = &*nextIter;

            // skip to token after "["
            iter += 2;

            Expression* subscriptExpr = ProcessExpression(iter, endIter, Token::eCloseBracket);
            if (subscriptExpr == nullptr)
            {
                return nullptr;
            }

            const Token* opToken2 = &*iter;

            expr = new BinaryExpression(BinaryExpression::eSubscript, expr, subscriptExpr, opToken, opToken2);

            nextIter = iter + 1;
        }
    }

    return expr;
}
