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
    {">>>", BinaryExpression::eShiftRightLogical},
    {"&", BinaryExpression::eBitwiseAnd},
    {"^", BinaryExpression::eBitwiseXor},
    {"|", BinaryExpression::eBitwiseOr},
    {"&&", BinaryExpression::eLogicalAnd},
    {"||", BinaryExpression::eLogicalOr},
    {ASSIGNMENT_OPERATOR, BinaryExpression::eAssign},
    {"+=", BinaryExpression::eAddAssign},
    {"-=", BinaryExpression::eSubtractAssign},
    {"*=", BinaryExpression::eMultiplyAssign},
    {"/=", BinaryExpression::eDivideAssign},
    {"%=", BinaryExpression::eRemainderAssign},
    {"<<=", BinaryExpression::eShiftLeftAssign},
    {">>=", BinaryExpression::eShiftRightArithmeticAssign},
    {">>>=", BinaryExpression::eShiftRightLogicalAssign},
    {"&=", BinaryExpression::eBitwiseAndAssign},
    {"^=", BinaryExpression::eBitwiseXorAssign},
    {"|=", BinaryExpression::eBitwiseOrAssign},
};

SyntaxAnalyzer::SyntaxAnalyzer(ErrorLogger& logger) :
    logger(logger)
{
}

bool SyntaxAnalyzer::Process(const TokenSequence& tokens, ModuleDefinition*& syntaxTree)
{
    TokenIterator iter = tokens.cbegin();
    TokenIterator endIter = tokens.cend();

    vector<TypeDefinition*> types;
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
        else if (iter->GetValue() == TYPE_KEYWORD)
        {
            TypeDefinition* typeDefinition = ProcessTypeDefinition(iter, endIter);
            if (typeDefinition == nullptr)
            {
                ok = false;
            }
            else
            {
                types.push_back(typeDefinition);
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
        syntaxTree = new ModuleDefinition(types, externFunctions, functions);
    }
    else
    {
        deletePointerContainer(types);
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

bool SyntaxAnalyzer::IncrementIteratorCheckValue(TokenIterator& iter, const TokenIterator& endIter, const string& expectedValue, const char* errorMsg)
{
    bool ok = IncrementIterator(iter, endIter, errorMsg);
    if (ok)
    {
        if (iter->GetValue() != expectedValue)
        {
            ok = false;
            logger.LogError(*iter, "Expected '{}'", expectedValue);
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

    string returnTypeName;

    // if no return type is specified, default to the unit type
    if (iter->GetValue() == endToken)
    {
        returnTypeName = "";
    }
    else // parse the return type
    {
        // get return type name
        returnTypeName = iter->GetValue();

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
        functionName, parameters, returnTypeName);
    return functionDeclaration;
}

bool SyntaxAnalyzer::ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                                       Parameters& parameters)
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
            Parameter* param = new Parameter(paramName, value);
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

TypeDefinition* SyntaxAnalyzer::ProcessTypeDefinition(TokenIterator& iter, TokenIterator endIter)
{
    if (!EndIteratorCheck(iter, endIter, "Expected type keyword"))
    {
        return nullptr;
    }

    if (iter->GetValue() != TYPE_KEYWORD)
    {
        logger.LogError("Expected type keyword");
    }

    if (!IncrementIterator(iter, endIter, "Expected type name"))
    {
        return nullptr;
    }

    if (!IsValidName(*iter))
    {
        logger.LogError(*iter, "'{}' is not a valid type name", iter->GetValue());
        return nullptr;
    }

    string typeName = iter->GetValue();

    if (!IncrementIterator(iter, endIter, "Expected '{'"))
    {
        return nullptr;
    }

    if (iter->GetValue() != "{")
    {
        logger.LogError(*iter, "Expected '{'");
        return nullptr;
    }

    // increment past "}"
    ++iter;

    vector<MemberDefinition*> members;
    while (iter != endIter && iter->GetValue() != "}")
    {
        // get member name
        if (!IsValidName(*iter))
        {
            deletePointerContainer(members);
            logger.LogError(*iter, "Invalid member name: '{}'", iter->GetValue());
            return nullptr;
        }
        const string& memberName = iter->GetValue();

        // get member type
        if (!IncrementIterator(iter, endIter, "Expected member type"))
        {
            deletePointerContainer(members);
            return nullptr;
        }
        const string& memberType = iter->GetValue();

        MemberDefinition* member = new MemberDefinition(memberName, memberType);
        members.push_back(member);

        if (!IncrementIterator(iter, endIter, "Expected ',' or '}'"))
        {
            deletePointerContainer(members);
            return nullptr;
        }

        const string& delimiter = iter->GetValue();
        if (delimiter == "}")
        {
            break;
        }
        else if (delimiter != ",")
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
    else if (iter->GetValue() != "}")
    {
        deletePointerContainer(members);
        logger.LogError(*iter, "Expected '}'");
        return nullptr;
    }

    // increment past "}"
    ++iter;

    TypeDefinition* typeDef = new TypeDefinition(typeName, members);
    return typeDef;
}

bool SyntaxAnalyzer::IsTypeInitialization(TokenIterator iter, TokenIterator endIter)
{
    if (iter == endIter)
    {
        return false;
    }

    ++iter;
    if (iter == endIter || iter->GetValue() != "{")
    {
        return false;
    }

    ++iter;
    if (iter == endIter)
    {
        return false;
    }
    else if (iter->GetValue() == "}")
    {
        return true;
    }
    else if (!IsValidName(*iter))
    {
        return false;
    }

    ++iter;
    if (iter == endIter || iter->GetValue() != ":")
    {
        return false;
    }

    return true;
}

TypeInitializationExpression* SyntaxAnalyzer::ProcessTypeInitialization(TokenIterator& iter, TokenIterator endIter)
{
    const string& typeName = iter->GetValue();

    // skip type name and '{'
    iter += 2;
    if (iter == endIter)
    {
        logger.LogError("Unexpected end of file in the middle of a type initialization");
        return nullptr;
    }

    // process member initializations
    vector<MemberInitialization*> members;
    while (iter != endIter && iter->GetValue() != "}")
    {
        // get member name
        if (!IsValidName(*iter))
        {
            deletePointerContainer(members);
            logger.LogError(*iter, "Invalid member name: '{}'", iter->GetValue());
            return nullptr;
        }
        const string& memberName = iter->GetValue();

        // make sure member name is followed by ':'
        if (!IncrementIteratorCheckValue(iter, endIter, ":", "Expected ':' after member"))
        {
            deletePointerContainer(members);
            return nullptr;
        }

        // get member expression
        ++iter;
        Expression* memberExpr = ProcessExpression(iter, endIter, {",", "}"});
        if (memberExpr == nullptr)
        {
            deletePointerContainer(members);
            return nullptr;
        }

        MemberInitialization* member = new MemberInitialization(memberName, memberExpr);
        members.push_back(member);

        const string& delimiter = iter->GetValue();
        if (delimiter == "}")
        {
            break;
        }
        else if (delimiter != ",")
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
    else if (iter->GetValue() != "}")
    {
        deletePointerContainer(members);
        logger.LogError(*iter, "Expected '}'");
        return nullptr;
    }

    TypeInitializationExpression* typeInit = new TypeInitializationExpression(typeName, members);
    return typeInit;
}

VariableDeclaration* SyntaxAnalyzer::ProcessVariableDeclaration(TokenIterator& iter, TokenIterator endIter)
{
    if (iter->GetValue() != VARIABLE_KEYWORD)
    {
        logger.LogError("Expected '{}'", VARIABLE_KEYWORD);
        return nullptr;
    }

    if (!IncrementIterator(iter, endIter, "Expected a variable name"))
    {
        return nullptr;
    }

    if (!IsValidName(*iter))
    {
        logger.LogError(*iter, "Invalid variable name");
        return nullptr;
    }

    string varName = iter->GetValue();

    if (!IncrementIterator(iter, endIter, "Expected variable type or assignment operator"))
    {
        return nullptr;
    }

    string varTypeName;
    if (iter->GetValue() == ASSIGNMENT_OPERATOR)
    {
        varTypeName = "";
    }
    else
    {
        varTypeName = iter->GetValue();

        if (!IncrementIterator(iter, endIter, "Expected an assignment operator"))
        {
            return nullptr;
        }

        if (iter->GetValue() != ASSIGNMENT_OPERATOR)
        {
            logger.LogError(*iter, "Expected an assignment operator");
            return nullptr;
        }
    }

    if (!IncrementIterator(iter, endIter, "Expected expression"))
    {
        return nullptr;
    }

    Expression* expression = ProcessExpression(iter, endIter, {STATEMENT_END, BLOCK_END});
    if (expression == nullptr)
    {
        return nullptr;
    }

    BinaryExpression* assignment = new BinaryExpression(BinaryExpression::eAssign, new VariableExpression(varName), expression);
    VariableDeclaration* varDecl = new VariableDeclaration(varName, varTypeName, assignment);
    return varDecl;
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

Expression* SyntaxAnalyzer::ProcessTerm(TokenIterator& iter, TokenIterator nextIter, TokenIterator endIter, bool& isPotentialEnd)
{
    string value = iter->GetValue();
    Expression* expr = nullptr;

    if (isNumber(value))
    {
        expr = new NumericExpression(value);
    }
    else if (isBool(value))
    {
        expr = new BoolLiteralExpression(value);
    }
    else if (value[0] == '"')
    {
        expr = ProcessStringExpression(iter);
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (value == "(")
    {
        TokenIterator parenEndIter = FindParenthesisEnd(iter, endIter);
        if (parenEndIter == endIter)
        {
            logger.LogError(*iter, "Could not find end parenthesis");
            return nullptr;
        }

        ++iter;
        expr = ProcessExpression(iter, parenEndIter, {});
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (value == BLOCK_START)
    {
        expr = ProcessBlockExpression(iter, endIter);
        if (expr == nullptr)
        {
            return nullptr;
        }
    }
    else if (value == IF_KEYWORD)
    {
        expr = ProcessBranchExpression(iter, endIter);
        if (expr == nullptr)
        {
            return nullptr;
        }

        isPotentialEnd = true;
    }
    else if (iter->GetValue() == WHILE_KEYWORD)
    {
        expr = ProcessWhileLoop(iter, endIter);
        if (expr == nullptr)
        {
            return nullptr;
        }

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
                logger.LogError("Unexpected end of file in the middle of a function call");
                return nullptr;
            }

            // process arguments
            vector<Expression*> arguments;
            while (iter->GetValue() != ")")
            {
                Expression* argExpr = ProcessExpression(iter, endIter, {",", ")"});
                if (argExpr == nullptr)
                {
                    deletePointerContainer(arguments);
                    return nullptr;
                }
                arguments.push_back(argExpr);

                if (iter->GetValue() == ",")
                {
                    ++iter;
                }
            }

            expr = new FunctionExpression(value, arguments);
        }
        // check if it's a type initialization
        else if (IsTypeInitialization(iter, endIter))
        {
            expr = ProcessTypeInitialization(iter, endIter);
            if (expr == nullptr)
            {
                return nullptr;
            }
        }
        else // it's a variable
        {
            expr = new VariableExpression(value);
        }
    }
    else
    {
        logger.LogError(*iter, "Unexpected term '{}'", value);
        return nullptr;
    }

    return expr;
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
                Expression* expr = ProcessTerm(iter, nextIter, endIter, isPotentialEnd);
                if (expr == nullptr)
                {
                    deletePointerContainer(terms);
                    return nullptr;
                }

                // update nextIter since iter may have changed in ProcessTerm()
                nextIter = (iter == endIter) ? endIter : iter + 1;

                if (nextIter != endIter && nextIter->GetValue() == ".")
                {
                    expr = ProcessMemberExpression(expr, iter, endIter);
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
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eShiftLeft, BinaryExpression::eShiftRightArithmetic, BinaryExpression::eShiftRightLogical});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseAnd});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseXor});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eBitwiseOr});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eLogicalAnd, BinaryExpression::eLogicalOr});
    ProcessExpressionOperators(terms, binOperators, {
            BinaryExpression::eAssign,
            BinaryExpression::eAddAssign,
            BinaryExpression::eSubtractAssign,
            BinaryExpression::eMultiplyAssign,
            BinaryExpression::eDivideAssign,
            BinaryExpression::eRemainderAssign,
            BinaryExpression::eShiftLeftAssign,
            BinaryExpression::eShiftRightArithmeticAssign,
            BinaryExpression::eShiftRightLogicalAssign,
            BinaryExpression::eBitwiseAndAssign,
            BinaryExpression::eBitwiseXorAssign,
            BinaryExpression::eBitwiseOrAssign,
        });

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

StringLiteralExpression* SyntaxAnalyzer::ProcessStringExpression(TokenIterator iter)
{
    const string& value = iter->GetValue();

    size_t idx = 0;
    if (value.size() < 1 || value[idx] != '"')
    {
        logger.LogError(*iter, "String doesn't start with '\"'");
        return nullptr;
    }

    vector<char> chars;
    chars.reserve(value.size());

    ++idx;
    size_t endCharsIdx = value.size() - 1;
    while (idx < endCharsIdx)
    {
        unsigned continuingByteCount = 0;

        char ch = value[idx];
        if (ch == '\\')
        {
            // make sure we're not at the end of the string
            if (idx >= endCharsIdx - 1)
            {
                logger.LogError(*iter, "Start of escape sequence at end of string");
                return nullptr;
            }

            ++idx;
            ch = value[idx];

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
            ch = value[idx];

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

    if (value[value.size() - 1] != '"')
    {
        logger.LogError(*iter, "String doesn't end with '\"'");
        return nullptr;
    }

    StringLiteralExpression* expr = new StringLiteralExpression(chars);
    return expr;
}

bool SyntaxAnalyzer::ProcessByteEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars)
{
    const string& value = iter->GetValue();
    size_t endCharsIdx = value.size() - 1;

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
    const string& value = iter->GetValue();
    size_t endCharsIdx = value.size() - 1;

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
    Expressions expressions;

    // increment iter past "{"
    ++iter;

    while (iter != endIter && iter->GetValue() != BLOCK_END)
    {
        Expression* expr = nullptr;

        if (iter->GetValue() == VARIABLE_KEYWORD)
        {
            expr = ProcessVariableDeclaration(iter, endIter);
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

    string nextValue = "";
    if (iter != endIter)
    {
        auto nextIter = iter + 1;
        if (nextIter != endIter)
        {
            nextValue = nextIter->GetValue();
        }
    }

    unique_ptr<Expression> elseExpression;
    if (nextValue == ELIF_KEYWORD)
    {
        // move to 'elif' keyword
        ++iter;

        // parse "elif"
        elseExpression.reset(ProcessBranchExpression(iter, endIter));
        if (elseExpression == nullptr)
        {
            return nullptr;
        }
    }
    else if (nextValue == ELSE_KEYWORD)
    {
        // move to 'else' keyword
        ++iter;

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
    else
    {
        // if there is no 'elif' or 'else' clause, set the else expression
        // to a unit type literal
        elseExpression.reset(new UnitTypeLiteralExpression());
    }

    BranchExpression* expr = new BranchExpression(ifCondition.release(),
                                                  ifExpression.release(),
                                                  elseExpression.release());
    return expr;
}

Expression* SyntaxAnalyzer::ProcessMemberExpression(Expression* expr, TokenIterator& iter, TokenIterator endIter)
{
    TokenIterator nextIter = iter + 1;

    while (nextIter != endIter && nextIter->GetValue() == ".")
    {
        // skip to token after "."
        iter += 2;

        if (iter == endIter)
        {
            logger.LogError(*iter, "No member name after member operator");
            delete expr;
            return nullptr;
        }
        else if (!IsValidName(*iter))
        {
            logger.LogError(*iter, "Invalid member name");
            delete expr;
            return nullptr;
        }

        expr = new MemberExpression(expr, iter->GetValue());

        nextIter = iter + 1;
    }

    return expr;
}
