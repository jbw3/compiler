#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "ErrorLogger.h"
#include "SyntaxTree.h"
#include "Token.h"
#include <map>
#include <stack>
#include <unordered_set>
#include <vector>

class SyntaxAnalyzer
{
public:
    typedef std::vector<Token> TokenSequence;
    typedef TokenSequence::const_iterator TokenIterator;

    static const std::string FUNCTION_KEYWORD;

    static const std::string IF_KEYWORD;

    static const std::string ELSE_KEYWORD;

    static const std::map<std::string, SyntaxTree::UnaryExpression::EOperator> UNARY_EXPRESSION_OPERATORS;

    static const std::map<std::string, SyntaxTree::BinaryExpression::EOperator> BINARY_EXPRESSION_OPERATORS;

    static const std::map<std::string, SyntaxTree::EType> TYPES;

    SyntaxAnalyzer(ErrorLogger& logger);

    bool Process(const TokenSequence& tokens, SyntaxTree::SyntaxTreeNode*& syntaxTree);

private:
    enum EParameterState
    {
        eName,
        eType,
        eDelimiter,
    };

    ErrorLogger& logger;

    bool EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool SkipNewlines(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::EType GetType(const std::string& typeName);

    SyntaxTree::FunctionDefinition* ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter);

    bool ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                           std::vector<SyntaxTree::VariableDefinition*>& parameters);

    SyntaxTree::Expression* AddUnaryExpressions(SyntaxTree::Expression* baseExpr,
                                                std::stack<SyntaxTree::UnaryExpression::EOperator>& unaryOperators);

    SyntaxTree::Expression* ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const std::unordered_set<std::string>& endTokens);

    void ProcessExpressionOperators(std::vector<SyntaxTree::Expression*>& terms,
                                    std::vector<SyntaxTree::BinaryExpression::EOperator>& operators,
                                    const std::unordered_set<SyntaxTree::BinaryExpression::EOperator>& opsToProcess);

    SyntaxTree::Expression* ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter);
};

#endif // SYNTAX_ANALYZER_H_
