#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <iostream>
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
};

const map<string, EType> SyntaxAnalyzer::TYPES =
{
    {"bool", EType::eBool},
    {"i32", EType::eInt32},
};

bool SyntaxAnalyzer::Process(const TokenSequence& tokens, SyntaxTreeNode*& syntaxTree)
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
        ++iter;
        if (iter == endIter)
        {
            cerr << "Did not expect end of file\n";
            return false;
        }
    } while (iter->GetValue() == "\n");

    return true;
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
    if (iter == endIter || iter->GetValue() != FUNCTION_KEYWORD)
    {
        cerr << "Expected function keyword\n";
        return nullptr;
    }

    ++iter;
    if (iter == endIter || !isIdentifier(iter->GetValue()))
    {
        cerr << "\"" << iter->GetValue() << "\" is not a valid function name\n";
        return nullptr;
    }

    string functionName = iter->GetValue();

    ++iter;
    if (iter == endIter || iter->GetValue() != "(")
    {
        cerr << "Expected \"(\"\n";
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

    // get return type
    ++iter;
    EType returnType = EType::eUnknown;
    if (iter == endIter)
    {
        ok = false;
    }
    else
    {
        returnType = GetType(iter->GetValue());
        if (returnType == EType::eUnknown)
        {
            ok = false;
        }
    }

    if (!ok)
    {
        cerr << "Expected return type\n";
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

    Expression* code = ProcessExpression(iter, endIter, {"\n"});
    if (code == nullptr)
    {
        deletePointerContainer(parameters);
        return nullptr;
    }

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
                cerr << "Invalid parameter name: \"" << value << "\"\n";
                return false;
            }

            state = eType;
        }
        else if (state == eType)
        {
            EType paramType = GetType(value);
            if (paramType == EType::eUnknown)
            {
                cerr << "\"" << value << "\" is not a known type\n";
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
                cerr << "Expected \",\" not \"" << value << "\"\n";
                return false;
            }

            state = eName;
        }
        else
        {
            cerr << "Internal error: Unknown state: " << state << "\n";
            return false;
        }

        ++iter;
    }

    if (iter == endIter)
    {
        cerr << "Expected \")\"\n";
        return false;
    }
    else if (state == eName && parameters.size() > 0)
    {
        cerr << "Expected a parameter\n";
        return false;
    }
    else if (state == eType)
    {
        cerr << "Expected a parameter type\n";
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
                            cerr << "Unexpected end of file\n";
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
                    cerr << "Unexpected term \"" << value << "\"\n";
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
                cerr << "Expected an operator, but got \"" << value << "\" instead\n";
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
        cerr << "Expected another expression term\n";
        deletePointerContainer(terms);
        return nullptr;
    }

    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eMultiply, BinaryExpression::eDivide, BinaryExpression::eModulo});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eAdd, BinaryExpression::eSubtract});
    ProcessExpressionOperators(terms, binOperators, {BinaryExpression::eEqual, BinaryExpression::eNotEqual, BinaryExpression::eLessThan, BinaryExpression::eLessThanOrEqual, BinaryExpression::eGreaterThan, BinaryExpression::eGreaterThanOrEqual});

    return terms.front();
}

void SyntaxAnalyzer::ProcessExpressionOperators(vector<Expression*>& terms,
                                                vector<BinaryExpression::EOperator>& operators,
                                                const unordered_set<BinaryExpression::EOperator>& opsToProcess)
{
    if (terms.size() != operators.size() + 1)
    {
        cerr << "Internal error: Expression terms and operators do not match\n";
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
    if (iter == endIter || iter->GetValue() != ELSE_KEYWORD)
    {
        cerr << "Expected 'else' keyword\n";
        return nullptr;
    }

    if (!SkipNewlines(iter, endIter))
    {
        return nullptr;
    }

    if (iter == endIter || (iter->GetValue() != IF_KEYWORD && iter->GetValue() != "{"))
    {
        cerr << "Expected 'if' or '{'\n";
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
