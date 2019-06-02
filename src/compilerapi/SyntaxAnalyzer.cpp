#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <memory>

using namespace std;
using namespace SyntaxTree;

const string SyntaxAnalyzer::FUNCTION_KEYWORD = "fun";

const string SyntaxAnalyzer::IF_KEYWORD = "if";

const string SyntaxAnalyzer::ELSE_KEYWORD = "else";

const map<string, UnaryExpression::EOperator> SyntaxAnalyzer::UNARY_EXPRESSION_OPERATORS =
{
    {"-", UnaryExpression::eNegative},
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
    {"&", BinaryExpression::eBitwiseAnd},
    {"^", BinaryExpression::eBitwiseXor},
    {"|", BinaryExpression::eBitwiseOr},
};

const map<string, EType> SyntaxAnalyzer::TYPES =
{
    {"bool", EType::eBool},
    {"i32", EType::eInt32},
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
        const string& tokenValue = iter->GetValue();
        if (tokenValue == "\n")
        {
            ++iter;
        }
        else
        {
            FunctionDefinition* functionDefinition = ProcessFunctionDefinition(iter, endIter);
            if (functionDefinition == nullptr)
            {
                ok = false;
            }
            else
            {
                functions.push_back(functionDefinition);
                if (iter != endIter)
                {
                    ++iter;
                }
            }
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

bool SyntaxAnalyzer::SkipNewlines(TokenIterator& iter, TokenIterator endIter)
{
    do
    {
        if (!IncrementIterator(iter, endIter))
        {
            return false;
        }
    } while (iter->GetValue() == "\n");

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

EType SyntaxAnalyzer::GetType(const std::string& typeName)
{
    auto typeIter = TYPES.find(typeName);
    if (typeIter == TYPES.cend())
    {
        return EType::eUnknown;
    }
    else
    {
        return typeIter->second;
    }
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
    EType returnType = GetType(iter->GetValue());
    if (returnType == EType::eUnknown)
    {
        logger.LogError(*iter, "Expected return type");
        deletePointerContainer(parameters);
        return nullptr;
    }

    // skip newlines
    ok = SkipNewlines(iter, endIter);
    if (!ok)
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
    if (!IncrementIterator(iter, endIter) || !SkipNewlines(iter, endIter))
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    Expression* code = ProcessExpression(iter, endIter, {"}", "\n"});
    if (code == nullptr)
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    if (!EndIteratorCheck(iter, endIter))
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    if (iter->GetValue() == "\n" && !SkipNewlines(iter, endIter))
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

    if (iter->GetValue() != "}")
    {
        logger.LogError(*iter, "Expected '}'");
        deletePointerContainer(parameters);
        return nullptr;
    }

    // increment past "}"
    ++iter;

    FunctionDefinition* functionDefinition = new FunctionDefinition(functionName, parameters, returnType, code);
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
            EType paramType = GetType(value);
            if (paramType == EType::eUnknown)
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
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseAnd});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseXor});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseOr});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual});

    return terms.front();
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
    unique_ptr<Expression> ifCondition(ProcessExpression(iter, endIter, {"{", "\n"}));
    if (ifCondition == nullptr)
    {
        return nullptr;
    }

    // increment past "{"
    ++iter;

    if (!SkipNewlines(iter, endIter))
    {
        return nullptr;
    }

    // read "if" expression
    unique_ptr<Expression> ifExpression(ProcessExpression(iter, endIter, {"}", "\n"}));
    if (ifExpression == nullptr)
    {
        return nullptr;
    }

    // increment past "}"
    ++iter;

    if (!SkipNewlines(iter, endIter))
    {
        return nullptr;
    }

    // we should be at the "else" clause
    if (!EndIteratorCheck(iter, endIter, "Expected 'else' keyword"))
    {
        return nullptr;
    }

    if (iter->GetValue() != ELSE_KEYWORD)
    {
        logger.LogError(*iter, "Expected 'else' keyword");
        return nullptr;
    }

    if (!SkipNewlines(iter, endIter))
    {
        return nullptr;
    }

    if (!EndIteratorCheck(iter, endIter, "Expected 'if' or '{'"))
    {
        return nullptr;
    }

    if (iter->GetValue() != IF_KEYWORD && iter->GetValue() != "{")
    {
        logger.LogError(*iter, "Expected 'if' or '{'");
        return nullptr;
    }

    unique_ptr<Expression> elseExpression;
    if (iter->GetValue() == IF_KEYWORD)
    {
        // parse "if else"
        elseExpression.reset(ProcessBranchExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        // increment past "{"
        ++iter;

        if (!SkipNewlines(iter, endIter))
        {
            return nullptr;
        }

        // read "else" expression
        elseExpression.reset(ProcessExpression(iter, endIter, {"}", "\n"}));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }

        // increment past "}"
        ++iter;
    }

    BranchExpression* expr = new BranchExpression(ifCondition.release(), ifExpression.release(), elseExpression.release());
    return expr;
}
