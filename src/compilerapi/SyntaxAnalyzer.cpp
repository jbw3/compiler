#include "SyntaxAnalyzer.h"
#include "keywords.h"
#include "utils.h"
#include <memory>

using namespace std;
using namespace SyntaxTree;

const string SyntaxAnalyzer::STATEMENT_END = ";";

const string SyntaxAnalyzer::BLOCK_START = "{";

const string SyntaxAnalyzer::BLOCK_END = "}";

const string SyntaxAnalyzer::ASSIGNMENT_OPERATOR = "=";

const map<string, UnaryExpression::EOperator> SyntaxAnalyzer::UNARY_EXPRESSION_OPERATORS =
{
    {"-", UnaryExpression::eNegative},
    {"!", UnaryExpression::eComplement},
};

const map<string, BinaryExpression::EOperator> SyntaxAnalyzer::BINARY_EXPRESSION_OPERATORS =
{
    {"==", BinaryExpression::eEqual},
    {"!=", BinaryExpression::eNotEqual},
    {"<", BinaryExpression::eLessThan},
    {"<=", BinaryExpression::eLessThanOrEqual},
    {">", BinaryExpression::eGreaterThan},
    {">=", BinaryExpression::eGreaterThanOrEqual},
    {"+", BinaryExpression::eAdd},
    {"-", BinaryExpression::eSubtract},
    {"*", BinaryExpression::eMultiply},
    {"/", BinaryExpression::eDivide},
    {"%", BinaryExpression::eRemainder},
    {"<<", BinaryExpression::eShiftLeft},
    {">>", BinaryExpression::eShiftRightArithmetic},
    {"&", BinaryExpression::eBitwiseAnd},
    {"^", BinaryExpression::eBitwiseXor},
    {"|", BinaryExpression::eBitwiseOr},
    {"&&", BinaryExpression::eLogicalAnd},
    {"||", BinaryExpression::eLogicalOr},
    {ASSIGNMENT_OPERATOR, BinaryExpression::eAssign},
};

SyntaxAnalyzer::SyntaxAnalyzer(ErrorLogger& logger) :
    logger(logger)
{
}

bool SyntaxAnalyzer::Process(const TokenSequence& tokens, ModuleDefinition*& syntaxTree)
{
    TokenIterator iter = tokens.cbegin();
    TokenIterator endIter = tokens.cend();

    vector<FunctionDefinition*> functions;
    vector<ExternFunctionDeclaration*> externFunctions;

    bool ok = true;
    while (ok && iter != endIter)
    {
        if (iter->GetValue() == FUNCTION_KEYWORD)
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
        else if (iter->GetValue() == EXTERN_KEYWORD)
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
            logger.LogError(*iter, "Unexpected token '{}'", iter->GetValue());
        }
    }

    if (ok)
    {
        syntaxTree = new ModuleDefinition(externFunctions, functions);
    }
    else
    {
        deletePointerContainer(functions);
        deletePointerContainer(externFunctions);
        syntaxTree = nullptr;
    }

    return ok;
}

bool SyntaxAnalyzer::IsValidName(const Token& name)
{
    const string& value = name.GetValue();

    if (!isIdentifier(value))
    {
        return false;
    }

    if (KEYWORDS.find(value) != KEYWORDS.cend())
    {
        return false;
    }

    if (RESERVED_KEYWORDS.find(value) != RESERVED_KEYWORDS.cend())
    {
        logger.LogWarning(name, "'{}' is a reserved keyword and may be an invalid identifier in a future version of the language", value);
    }

    return true;
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


ExternFunctionDeclaration* SyntaxAnalyzer::ProcessExternFunction(TokenIterator& iter,
                                                                 TokenIterator endIter)
{
    if (!EndIteratorCheck(iter, endIter, "Expected extern keyword"))
    {
        return nullptr;
    }

    if (iter->GetValue() != EXTERN_KEYWORD)
    {
        logger.LogError(*iter, "Expected extern keyword");
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected function keyword"))
    {
        return nullptr;
    }

    FunctionDeclaration* decl = ProcessFunctionDeclaration(iter, endIter, STATEMENT_END);
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
    unique_ptr<FunctionDeclaration> functionDeclaration(ProcessFunctionDeclaration(iter, endIter, BLOCK_START));
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

    if (iter->GetValue() != "}")
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
                                                                const string& endToken)
{
    if (!EndIteratorCheck(iter, endIter, "Expected function keyword"))
    {
        return nullptr;
    }

    if (iter->GetValue() != FUNCTION_KEYWORD)
    {
        logger.LogError(*iter, "Expected function keyword");
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected function name"))
    {
        return nullptr;
    }

    if (!IsValidName(*iter))
    {
        logger.LogError(*iter, "'{}' is not a valid function name", iter->GetValue());
        return nullptr;
    }

    string functionName = iter->GetValue();

    if (!IncrementIterator(iter, endIter, "Expected '('"))
    {
        return nullptr;
    }

    if (iter->GetValue() != "(")
    {
        logger.LogError(*iter, "Expected '('");
        return nullptr;
    }

    ++iter;
    VariableDeclarations parameters;
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

    const TypeInfo* returnType = nullptr;

    // if no return type is specified, default to the unit type
    if (iter->GetValue() == endToken)
    {
        returnType = TypeInfo::UnitType;
    }
    else // parse the return type
    {
        // get return type
        returnType = TypeInfo::GetType(iter->GetValue());
        if (returnType == nullptr)
        {
            logger.LogError(*iter, "Expected return type");
            deletePointerContainer(parameters);
            return nullptr;
        }

        if (!IncrementIterator(iter, endIter, "Expected end of function"))
        {
            deletePointerContainer(parameters);
            return nullptr;
        }

        if (iter->GetValue() != endToken)
        {
            logger.LogError(*iter, "Expected '{}'", endToken);
            deletePointerContainer(parameters);
            return nullptr;
        }
    }

    FunctionDeclaration* functionDeclaration = new FunctionDeclaration(
        functionName, parameters, returnType);
    return functionDeclaration;
}

bool SyntaxAnalyzer::ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                                       VariableDeclarations& parameters)
{
    EParameterState state = eName;
    parameters.clear();

    string paramName;
    while (iter != endIter && iter->GetValue() != ")")
    {
        const string& value = iter->GetValue();
        if (state == eName)
        {
            if (IsValidName(*iter))
            {
                paramName = value;
            }
            else
            {
                logger.LogError(*iter, "Invalid parameter name: '{}'", value);
                return false;
            }

            state = eType;
        }
        else if (state == eType)
        {
            const TypeInfo* paramType = TypeInfo::GetType(value);
            if (paramType == nullptr)
            {
                logger.LogError(*iter, "'{}' is not a known type", value);
                return false;
            }

            VariableDeclaration* param = new VariableDeclaration(paramName, paramType);
            parameters.push_back(param);

            state = eDelimiter;
        }
        else if (state == eDelimiter)
        {
            if (value != ",")
            {
                logger.LogError(*iter, "Expected ',' not '{}'", value);
                return false;
            }

            state = eName;
        }
        else
        {
            logger.LogError(*iter, "Internal error: Unknown state: {}", state);
            return false;
        }

        ++iter;
    }

    if (iter == endIter)
    {
        logger.LogError("Expected ')'");
        return false;
    }
    else if (state == eName && parameters.size() > 0)
    {
        logger.LogError(*iter, "Expected a parameter");
        return false;
    }
    else if (state == eType)
    {
        logger.LogError(*iter, "Expected a parameter type");
        return false;
    }

    return true;
}

void SyntaxAnalyzer::ProcessVariableDeclaration(TokenIterator& iter, TokenIterator endIter, VariableDeclaration*& varDecl, Expression*& assignment)
{
    varDecl = nullptr;
    assignment = nullptr;

    if (iter->GetValue() != VARIABLE_KEYWORD)
    {
        logger.LogError("Expected '{}'", VARIABLE_KEYWORD);
        return;
    }

    if (!IncrementIterator(iter, endIter, "Expected a variable name"))
    {
        return;
    }

    if (!IsValidName(*iter))
    {
        logger.LogError(*iter, "Invalid variable name");
        return;
    }

    string varName = iter->GetValue();

    if (!IncrementIterator(iter, endIter, "Expected variable type"))
    {
        return;
    }

    string varTypeName = iter->GetValue();
    const TypeInfo* varType = TypeInfo::GetType(varTypeName);
    if (varType == nullptr)
    {
        logger.LogError(*iter, "'{}' is not a known type", varTypeName);
        return;
    }

    if (!IncrementIterator(iter, endIter, "Expected operator"))
    {
        return;
    }

    if (iter->GetValue() != ASSIGNMENT_OPERATOR)
    {
        logger.LogError(*iter, "Expected an assignment operator");
        return;
    }

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        return;
    }

    Expression* expression = ProcessExpression(iter, endIter, {STATEMENT_END, BLOCK_END});
    if (expression == nullptr)
    {
        return;
    }

    varDecl = new VariableDeclaration(varName, varType);
    assignment = new BinaryExpression(BinaryExpression::eAssign, new VariableExpression(varName), expression);
}

WhileLoop* SyntaxAnalyzer::ProcessWhileLoop(TokenIterator& iter, TokenIterator endIter)
{
    // increment iter past "while" keyword
    ++iter;

    // read "while" condition
    unique_ptr<Expression> whileCondition(ProcessExpression(iter, endIter, {BLOCK_START}));
    if (whileCondition == nullptr)
    {
        return nullptr;
    }

    if (iter == endIter || iter->GetValue() != BLOCK_START)
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    unique_ptr<Expression> expression(ProcessBlockExpression(iter, endIter));
    if (expression == nullptr)
    {
        return nullptr;
    }

    WhileLoop* whileLoop = new WhileLoop(whileCondition.release(), expression.release());
    return whileLoop;
}

SyntaxAnalyzer::TokenIterator SyntaxAnalyzer::FindStatementEnd(TokenIterator iter, TokenIterator endIter)
{
    unsigned int balance = 0;

    while (iter != endIter)
    {
        const string& value = iter->GetValue();
        if (balance == 0)
        {
            if (value == ";" || value == "}")
            {
                break;
            }
        }
        else if (value == "{")
        {
            ++balance;
        }
        else if (value == "}")
        {
            --balance;
        }

        ++iter;
    }

    return iter;
}

Expression* SyntaxAnalyzer::AddUnaryExpressions(Expression* baseExpr, stack<UnaryExpression::EOperator>& unaryOperators)
{
    Expression* result = baseExpr;
    while (!unaryOperators.empty())
    {
        result = new UnaryExpression(unaryOperators.top(), result);
        unaryOperators.pop();
    }

    return result;
}

Expression* SyntaxAnalyzer::ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const unordered_set<string>& endTokens)
{
    bool expectTerm = true;
    bool isEnd = false;
    bool isPotentialEnd = false;
    vector<Expression*> terms;
    stack<UnaryExpression::EOperator> unaryOperators;
    vector<BinaryExpression::EOperator> binOperators;

    while (iter != endIter)
    {
        string value = iter->GetValue();
        TokenIterator nextIter = iter + 1;

        if (endTokens.find(value) != endTokens.cend())
        {
            break;
        }

        isEnd = false;
        if (expectTerm)
        {
            isPotentialEnd = false;

            auto unaryOpIter = UNARY_EXPRESSION_OPERATORS.find(value);
            if (unaryOpIter != UNARY_EXPRESSION_OPERATORS.cend())
            {
                unaryOperators.push(unaryOpIter->second);
                expectTerm = true;
            }
            else
            {
                if (isNumber(value))
                {
                    Expression* expr = AddUnaryExpressions(new NumericExpression(value), unaryOperators);
                    terms.push_back(expr);
                }
                else if (isBool(value))
                {
                    Expression* expr = AddUnaryExpressions(new BoolLiteralExpression(value), unaryOperators);
                    terms.push_back(expr);
                }
                else if (value == "(")
                {
                    TokenIterator parenEndIter = FindParenthesisEnd(iter, endIter);
                    if (parenEndIter == endIter)
                    {
                        logger.LogError(*iter, "Could not find end parenthesis");
                        deletePointerContainer(terms);
                        return nullptr;
                    }

                    ++iter;
                    Expression* expr = ProcessExpression(iter, parenEndIter, {});
                    if (expr == nullptr)
                    {
                        deletePointerContainer(terms);
                        return nullptr;
                    }

                    expr = AddUnaryExpressions(expr, unaryOperators);
                    terms.push_back(expr);
                }
                else if (value == BLOCK_START)
                {
                    Expression* expr = ProcessBlockExpression(iter, endIter);
                    if (expr == nullptr)
                    {
                        deletePointerContainer(terms);
                        return nullptr;
                    }

                    expr = AddUnaryExpressions(expr, unaryOperators);
                    terms.push_back(expr);
                }
                else if (value == IF_KEYWORD)
                {
                    Expression* expr = ProcessBranchExpression(iter, endIter);
                    if (expr == nullptr)
                    {
                        deletePointerContainer(terms);
                        return nullptr;
                    }

                    expr = AddUnaryExpressions(expr, unaryOperators);
                    terms.push_back(expr);

                    isPotentialEnd = true;
                }
                else if (iter->GetValue() == WHILE_KEYWORD)
                {
                    Expression* expr = ProcessWhileLoop(iter, endIter);
                    if (expr == nullptr)
                    {
                        deletePointerContainer(terms);
                        return nullptr;
                    }

                    expr = AddUnaryExpressions(expr, unaryOperators);
                    terms.push_back(expr);

                    isPotentialEnd = true;
                }
                else if (IsValidName(*iter))
                {
                    // check if it's a function call
                    if (nextIter != endIter && nextIter->GetValue() == "(")
                    {
                        iter += 2;
                        if (iter == endIter)
                        {
                            logger.LogError("Unexpected end of file");
                            deletePointerContainer(terms);
                            return nullptr;
                        }

                        // process arguments
                        vector<Expression*> arguments;
                        while (iter->GetValue() != ")")
                        {
                            Expression* argExpr = ProcessExpression(iter, endIter, {",", ")"});
                            if (argExpr == nullptr)
                            {
                                deletePointerContainer(terms);
                                deletePointerContainer(arguments);
                                return nullptr;
                            }
                            arguments.push_back(argExpr);

                            if (iter->GetValue() == ",")
                            {
                                ++iter;
                            }
                        }

                        Expression* expr = AddUnaryExpressions(new FunctionExpression(value, arguments), unaryOperators);
                        terms.push_back(expr);
                    }
                    else // it's a variable
                    {
                        Expression* expr = AddUnaryExpressions(new VariableExpression(value), unaryOperators);
                        terms.push_back(expr);
                    }
                }
                else
                {
                    logger.LogError(*iter, "Unexpected term '{}'", value);
                    deletePointerContainer(terms);
                    return nullptr;
                }

                expectTerm = false;
            }
        }
        else
        {
            auto opIter = BINARY_EXPRESSION_OPERATORS.find(value);

            // if the token is a binary operator, add it to the list
            if (opIter != BINARY_EXPRESSION_OPERATORS.cend())
            {
                binOperators.push_back(opIter->second);
                expectTerm = true;
            }
            // if we are at the end of an expression, we're done
            else if (isPotentialEnd)
            {
                break;
            }
            else
            {
                logger.LogError(*iter, "Expected an operator, but got '{}' instead", value);
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

    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eMultiply, BinaryExpression::eDivide, BinaryExpression::eRemainder});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eAdd, BinaryExpression::eSubtract});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eShiftLeft, BinaryExpression::eShiftRightArithmetic});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseAnd});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseXor});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseOr});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eLogicalAnd, BinaryExpression::eLogicalOr});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eAssign});

    return terms.front();
}

SyntaxAnalyzer::TokenIterator SyntaxAnalyzer::FindParenthesisEnd(TokenIterator iter, TokenIterator endIter)
{
    unsigned int balance = 1;

    ++iter;
    while (iter != endIter)
    {
        if (iter->GetValue() == "(")
        {
            ++balance;
        }
        else if (iter->GetValue() == ")")
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
                                                vector<BinaryExpression::EOperator>& operators,
                                                const unordered_set<BinaryExpression::EOperator>& opsToProcess)
{
    if (terms.size() != operators.size() + 1)
    {
        logger.LogError("Internal error: Expression terms and operators do not match");
        return;
    }

    auto term1Iter = terms.begin();
    auto opIter = operators.begin();
    while (opIter != operators.end())
    {
        if (opsToProcess.find(*opIter) != opsToProcess.cend())
        {
            auto term2Iter = term1Iter + 1;
            *term1Iter = new BinaryExpression(*opIter, *term1Iter, *term2Iter);

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

BlockExpression* SyntaxAnalyzer::ProcessBlockExpression(TokenIterator& iter, TokenIterator endIter)
{
    bool needsUnitType = true;
    Expressions expressions;

    // increment iter past "{"
    ++iter;

    while (iter != endIter && iter->GetValue() != BLOCK_END)
    {
        Expression* expr = nullptr;

        if (iter->GetValue() == VARIABLE_KEYWORD)
        {
            VariableDeclaration* varDecl = nullptr;
            ProcessVariableDeclaration(iter, endIter, varDecl, expr);

            if (varDecl != nullptr)
            {
                expressions.push_back(varDecl);
            }
        }
        else
        {
            // process the sub-expression
            expr = ProcessExpression(iter, endIter, {STATEMENT_END, BLOCK_END});
        }

        // if there was an error, return null
        if (expr == nullptr)
        {
            deletePointerContainer(expressions);
            return nullptr;
        }

        expressions.push_back(expr);

        // if we reached the end, log an error and return null
        if (!EndIteratorCheck(iter, endIter, "Expected block end"))
        {
            deletePointerContainer(expressions);
            return nullptr;
        }

        // if we reached the end of a statement, increment the iterator
        if (iter->GetValue() == STATEMENT_END)
        {
            ++iter;
        }
        // if we reached the end of a block, we're done, and the last expression is the
        // block's return type (so we don't need the unit type expression)
        else if (iter->GetValue() == BLOCK_END)
        {
            needsUnitType = false;
        }
    }

    if (!EndIteratorCheck(iter, endIter, "Expected block end"))
    {
        deletePointerContainer(expressions);
        return nullptr;
    }

    if (needsUnitType)
    {
        expressions.push_back(new UnitTypeLiteralExpression());
    }

    BlockExpression* blockExpression = new BlockExpression(expressions);
    return blockExpression;
}

Expression* SyntaxAnalyzer::ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter)
{
    // increment iter past "if" or "elif" keyword
    ++iter;

    // read "if" condition
    unique_ptr<Expression> ifCondition(ProcessExpression(iter, endIter, {BLOCK_START}));
    if (ifCondition == nullptr)
    {
        return nullptr;
    }

    if (iter == endIter || iter->GetValue() != BLOCK_START)
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

    // increment past "}"; we should be at an "elif" or "else" clause
    if (!IncrementIterator(iter, endIter, "Expected 'elif' or 'else' keyword"))
    {
        return nullptr;
    }

    if (iter->GetValue() != ELIF_KEYWORD && iter->GetValue() != ELSE_KEYWORD)
    {
        logger.LogError(*iter, "Expected 'elif' or 'else'");
        return nullptr;
    }

    unique_ptr<Expression> elseExpression;
    if (iter->GetValue() == ELIF_KEYWORD)
    {
        // parse "elif"
        elseExpression.reset(ProcessBranchExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        if (!IncrementIterator(iter, endIter, "Expected '{'"))
        {
            return nullptr;
        }

        if (iter->GetValue() != BLOCK_START)
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

    BranchExpression* expr = new BranchExpression(ifCondition.release(),
                                                  ifExpression.release(),
                                                  elseExpression.release());
    return expr;
}
