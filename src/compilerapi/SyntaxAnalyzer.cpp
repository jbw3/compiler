#include "SyntaxAnalyzer.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace SyntaxTree;

const string SyntaxAnalyzer::FUNCTION_KEYWORD = "fun";

const map<string, BinaryExpression::EOperator> SyntaxAnalyzer::BINARY_EXPRESSION_OPERATORS =
{
    {"+", BinaryExpression::eAdd},
    {"-", BinaryExpression::eSubtract},
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
        for (auto param : parameters)
        {
            delete param;
        }
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
        for (auto param : parameters)
        {
            delete param;
        }
        return nullptr;
    }

    // skip newlines
    ok = SkipNewlines(iter, endIter);
    if (!ok)
    {
        for (auto param : parameters)
        {
            delete param;
        }
        return nullptr;
    }

    Expression* code = ProcessExpression(iter, endIter, {"\n"});
    if (code == nullptr)
    {
        for (auto param : parameters)
        {
            delete param;
        }
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

Expression* SyntaxAnalyzer::ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const unordered_set<string>& endTokens)
{
    Expression* expression = nullptr;
    bool expectNumOrVar = true;
    BinaryExpression::EOperator binOp = BinaryExpression::eAdd;
    while (iter != endIter)
    {
        string value = iter->GetValue();

        if (endTokens.find(value) != endTokens.cend())
        {
            break;
        }

        if (expectNumOrVar)
        {
            if (isNumber(value))
            {
                NumericExpression* numExpr = new NumericExpression(value);
                if (expression == nullptr)
                {
                    expression = numExpr;
                }
                else
                {
                    BinaryExpression* binExpr = new BinaryExpression(binOp, expression, numExpr);
                    expression = binExpr;
                }
            }
            else if (isBool(value))
            {
                BoolLiteralExpression* boolExpr = new BoolLiteralExpression(value);
                if (expression == nullptr)
                {
                    expression = boolExpr;
                }
                else
                {
                    BinaryExpression* binExpr = new BinaryExpression(binOp, expression, boolExpr);
                    expression = binExpr;
                }
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
                        delete expression;
                        return nullptr;
                    }

                    // process arguments
                    vector<Expression*> arguments;
                    while (iter->GetValue() != ")")
                    {
                        Expression* argExpr = ProcessExpression(iter, endIter, {",", ")"});
                        if (argExpr == nullptr)
                        {
                            delete expression;
                            for (Expression* arg : arguments)
                            {
                                delete arg;
                            }
                            return nullptr;
                        }
                        arguments.push_back(argExpr);

                        if (iter->GetValue() == ",")
                        {
                            ++iter;
                        }
                    }

                    FunctionExpression* funcExpr = new FunctionExpression(value, arguments);
                    if (expression == nullptr)
                    {
                        expression = funcExpr;
                    }
                    else
                    {
                        BinaryExpression* binExpr =
                            new BinaryExpression(binOp, expression, funcExpr);
                        expression = binExpr;
                    }
                }
                else // it's a variable
                {
                    VariableExpression* varExpr = new VariableExpression(value);
                    if (expression == nullptr)
                    {
                        expression = varExpr;
                    }
                    else
                    {
                        BinaryExpression* binExpr =
                            new BinaryExpression(binOp, expression, varExpr);
                        expression = binExpr;
                    }
                }
            }
            else
            {
                cerr << "Unexpected term \"" << value << "\"\n";
                delete expression;
                return nullptr;
            }
        }
        else
        {
            auto opIter = BINARY_EXPRESSION_OPERATORS.find(value);
            if (opIter == BINARY_EXPRESSION_OPERATORS.cend())
            {
                cerr << "Expected an operator, but got \"" << value << "\" instead\n";
                delete expression;
                return nullptr;
            }

            binOp = opIter->second;
        }

        expectNumOrVar = !expectNumOrVar;
        ++iter;
    }

    if (expectNumOrVar)
    {
        cerr << "Expected another number\n";
        delete expression;
        return nullptr;
    }

    return expression;
}