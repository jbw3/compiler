#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <memory>

using namespace std;
using namespace SyntaxTree;

const string SyntaxAnalyzer::FUNCTION_KEYWORD = "fun";

const string SyntaxAnalyzer::VARIABLE_KEYWORD = "var";

const string SyntaxAnalyzer::ASSIGNMENT_OPERATOR = "=";

const string SyntaxAnalyzer::STATEMENT_END = ";";

const string SyntaxAnalyzer::IF_KEYWORD = "if";

const string SyntaxAnalyzer::ELSE_KEYWORD = "else";

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
    {"%", BinaryExpression::eModulo},
    {"<<", BinaryExpression::eShiftLeft},
    {">>", BinaryExpression::eShiftRightArithmetic},
    {"&", BinaryExpression::eBitwiseAnd},
    {"^", BinaryExpression::eBitwiseXor},
    {"|", BinaryExpression::eBitwiseOr},
    {"&&", BinaryExpression::eLogicalAnd},
    {"||", BinaryExpression::eLogicalOr},
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

    bool ok = true;
    while (ok && iter != endIter)
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

    if (ok)
    {
        syntaxTree = new ModuleDefinition(functions);
    }
    else
    {
        for (FunctionDefinition* funcDef : functions)
        {
            delete funcDef;
        }
        functions.clear();
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

FunctionDefinition* SyntaxAnalyzer::ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter)
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

    if (!isIdentifier(iter->GetValue()))
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
    vector<VariableDefinition*> parameters;
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

    // get return type
    const TypeInfo* returnType = TypeInfo::GetType(iter->GetValue());
    if (returnType == nullptr)
    {
        logger.LogError(*iter, "Expected return type");
        deletePointerContainer(parameters);
        return nullptr;
    }

    // skip newlines
    if (!IncrementIterator(iter, endIter, "Expected '{'"))
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    if (iter->GetValue() != "{")
    {
        logger.LogError(*iter, "Expected '{'");
        deletePointerContainer(parameters);
        return nullptr;
    }

    // increment past "{" and skip newlines
    if (!IncrementIterator(iter, endIter))
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    Statements statements;
    ok = ProcessStatements(statements, iter, endIter);
    if (!ok)
    {
        deletePointerContainer(parameters);
        deletePointerContainer(statements);
        return nullptr;
    }

    Expression* returnExpression = ProcessExpression(iter, endIter, {"}"});
    if (returnExpression == nullptr)
    {
        deletePointerContainer(parameters);
        deletePointerContainer(statements);
        return nullptr;
    }

    if (!EndIteratorCheck(iter, endIter))
    {
        deletePointerContainer(parameters);
        deletePointerContainer(statements);
        return nullptr;
    }

    if (iter->GetValue() != "}")
    {
        logger.LogError(*iter, "Expected '}'");
        deletePointerContainer(parameters);
        deletePointerContainer(statements);
        return nullptr;
    }

    // increment past "}"
    ++iter;

    FunctionDefinition* functionDefinition = new FunctionDefinition(
        functionName, parameters, returnType, statements, returnExpression
    );
    return functionDefinition;
}

bool SyntaxAnalyzer::ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                                       vector<VariableDefinition*>& parameters)
{
    EParameterState state = eName;
    parameters.clear();

    string paramName;
    while (iter != endIter && iter->GetValue() != ")")
    {
        const string& value = iter->GetValue();
        if (state == eName)
        {
            if (isIdentifier(value))
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

            VariableDefinition* param = new VariableDefinition(paramName, paramType);
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

Assignment* SyntaxAnalyzer::ProcessAssignment(TokenIterator& iter, TokenIterator endIter)
{
    string varName;

    // make sure the statement is not empty
    if (iter == endIter)
    {
        logger.LogError("Empty statement");
        return nullptr;
    }

    if (!isIdentifier(iter->GetValue()))
    {
        logger.LogError(*iter, "Expected a variable name");
        return nullptr;
    }

    varName = iter->GetValue();

    if (!IncrementIterator(iter, endIter, "Expected operator"))
    {
        return nullptr;
    }

    if (iter->GetValue() != ASSIGNMENT_OPERATOR)
    {
        logger.LogError(*iter, "Expected an assignment operator");
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        return nullptr;
    }

    Expression* expression = ProcessExpression(iter, endIter, {STATEMENT_END});
    if (expression == nullptr)
    {
        return nullptr;
    }

    // increment past ";"
    if (!IncrementIterator(iter, endIter))
    {
        return nullptr;
    }

    Assignment* assignment = new Assignment(varName, expression);
    return assignment;
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
    vector<Expression*> terms;
    stack<UnaryExpression::EOperator> unaryOperators;
    vector<BinaryExpression::EOperator> binOperators;

    while (iter != endIter)
    {
        string value = iter->GetValue();

        if (endTokens.find(value) != endTokens.cend())
        {
            break;
        }

        if (expectTerm)
        {
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
                }
                else if (isIdentifier(value))
                {
                    // check if it's a function call
                    TokenIterator next = iter + 1;
                    if (next != endIter && next->GetValue() == "(")
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
            if (opIter == BINARY_EXPRESSION_OPERATORS.cend())
            {
                logger.LogError(*iter, "Expected an operator, but got '{}' instead", value);
                deletePointerContainer(terms);
                return nullptr;
            }

            binOperators.push_back(opIter->second);
            expectTerm = true;
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

    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eMultiply, BinaryExpression::eDivide, BinaryExpression::eModulo});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eAdd, BinaryExpression::eSubtract});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eShiftLeft, BinaryExpression::eShiftRightArithmetic});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseAnd});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseXor});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseOr});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eLogicalAnd, BinaryExpression::eLogicalOr});

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

Expression* SyntaxAnalyzer::ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter)
{
    // increment iter past "if" keyword
    ++iter;

    // read "if" condition
    unique_ptr<Expression> ifCondition(ProcessExpression(iter, endIter, {"{"}));
    if (ifCondition == nullptr)
    {
        return nullptr;
    }

    // increment past "{"
    if (!IncrementIterator(iter, endIter))
    {
        return nullptr;
    }

    // read "if" statements
    Statements ifStatements;
    bool ok = ProcessStatements(ifStatements, iter, endIter);
    if (!ok)
    {
        return nullptr;
    }

    // read "if" expression
    unique_ptr<Expression> ifExpression(ProcessExpression(iter, endIter, {"}"}));
    if (ifExpression == nullptr)
    {
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    // increment past "}"
    if (!IncrementIterator(iter, endIter))
    {
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    // we should be at the "else" clause
    if (!EndIteratorCheck(iter, endIter, "Expected 'else' keyword"))
    {
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    if (iter->GetValue() != ELSE_KEYWORD)
    {
        logger.LogError(*iter, "Expected 'else' keyword");
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected 'if' or '{'"))
    {
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    if (iter->GetValue() != IF_KEYWORD && iter->GetValue() != "{")
    {
        logger.LogError(*iter, "Expected 'if' or '{'");
        deletePointerContainer(ifStatements);
        return nullptr;
    }

    Statements elseStatements;
    unique_ptr<Expression> elseExpression;
    if (iter->GetValue() == IF_KEYWORD)
    {
        // parse "if else"
        elseExpression.reset(ProcessBranchExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            deletePointerContainer(ifStatements);
            return nullptr;
        }
    }
    else
    {
        // increment past "{"
        if (!IncrementIterator(iter, endIter))
        {
            deletePointerContainer(ifStatements);
            return nullptr;
        }

        // read "else" statements
        bool ok = ProcessStatements(elseStatements, iter, endIter);
        if (!ok)
        {
            deletePointerContainer(ifStatements);
            deletePointerContainer(elseStatements);
            return nullptr;
        }

        // read "else" expression
        elseExpression.reset(ProcessExpression(iter, endIter, {"}"}));
        if (elseExpression == nullptr)
        {
            deletePointerContainer(ifStatements);
            deletePointerContainer(elseStatements);
            return nullptr;
        }
    }

    BranchExpression* expr = new BranchExpression(ifCondition.release(),
                                                  ifStatements, ifExpression.release(),
                                                  elseStatements, elseExpression.release());
    return expr;
}

bool SyntaxAnalyzer::ProcessStatements(Statements& statements, TokenIterator& iter, TokenIterator endIter)
{
    statements.clear();
    statements.reserve(8);

    while (iter != endIter && (iter + 1) != endIter && (iter + 1)->GetValue() == ASSIGNMENT_OPERATOR)
    {
        SyntaxTreeNode* statement = ProcessAssignment(iter, endIter);
        if (statement == nullptr)
        {
            deletePointerContainer(statements);
            return false;
        }

        statements.push_back(statement);
    }

    return true;
}
