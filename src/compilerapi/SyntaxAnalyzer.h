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

    static const std::string STATEMENT_END;

    static const std::string BLOCK_START;

    static const std::string BLOCK_END;

    static const std::string ASSIGNMENT_OPERATOR;

    static const std::map<std::string, SyntaxTree::UnaryExpression::EOperator> UNARY_EXPRESSION_OPERATORS;

    static const std::map<std::string, SyntaxTree::BinaryExpression::EOperator> BINARY_EXPRESSION_OPERATORS;

    SyntaxAnalyzer(ErrorLogger& logger);

    bool Process(const TokenSequence& tokens, SyntaxTree::ModuleDefinition*& syntaxTree);

private:
    enum EParameterState
    {
        eName,
        eType,
        eDelimiter,
    };

    bool IsValidName(const Token& name);

    ErrorLogger& logger;
    SyntaxTree::VariableDefinitions variableDefinitions;

    bool EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    SyntaxTree::FunctionDefinition* ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter);

    bool ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                           std::vector<SyntaxTree::VariableDefinition*>& parameters);

    SyntaxTree::Assignment* ProcessAssignment(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::WhileLoop* ProcessWhileLoop(TokenIterator& iter, TokenIterator endIter);

    TokenIterator FindStatementEnd(TokenIterator iter, TokenIterator endIter);

    SyntaxTree::Expression* AddUnaryExpressions(SyntaxTree::Expression* baseExpr,
                                                std::stack<SyntaxTree::UnaryExpression::EOperator>& unaryOperators);

    SyntaxTree::Expression* ProcessBlockEndExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const std::unordered_set<std::string>& endTokens);

    TokenIterator FindParenthesisEnd(TokenIterator iter, TokenIterator endIter);

    void ProcessExpressionOperators(std::vector<SyntaxTree::Expression*>& terms,
                                    std::vector<SyntaxTree::BinaryExpression::EOperator>& operators,
                                    const std::unordered_set<SyntaxTree::BinaryExpression::EOperator>& opsToProcess);

    SyntaxTree::BlockExpression* ProcessBlockExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter);

    bool ProcessStatements(SyntaxTree::Statements& statements, TokenIterator& iter, TokenIterator endIter);
};

#endif // SYNTAX_ANALYZER_H_
