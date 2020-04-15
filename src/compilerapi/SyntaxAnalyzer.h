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

    bool EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIteratorCheckValue(TokenIterator& iter, const TokenIterator& endIter, const std::string& expectedValue, const char* errorMsg = nullptr);

    SyntaxTree::ExternFunctionDeclaration* ProcessExternFunction(TokenIterator& iter,
                                                                 TokenIterator endIter);

    SyntaxTree::FunctionDefinition* ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter);

    SyntaxTree::FunctionDeclaration* ProcessFunctionDeclaration(TokenIterator& iter,
                                                                TokenIterator endIter,
                                                                const std::string& endToken);

    bool ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                           SyntaxTree::Parameters& parameters);

    SyntaxTree::TypeDefinition* ProcessTypeDefinition(TokenIterator& iter,
                                                      TokenIterator endIter);

    bool IsTypeInitialization(TokenIterator iter, TokenIterator endIter);

    SyntaxTree::TypeInitializationExpression* ProcessTypeInitialization(TokenIterator& iter,
                                                                        TokenIterator endIter);

    SyntaxTree::VariableDeclaration* ProcessVariableDeclaration(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::WhileLoop* ProcessWhileLoop(TokenIterator& iter, TokenIterator endIter);

    TokenIterator FindStatementEnd(TokenIterator iter, TokenIterator endIter);

    SyntaxTree::Expression* AddUnaryExpressions(SyntaxTree::Expression* baseExpr,
                                                std::stack<SyntaxTree::UnaryExpression::EOperator>& unaryOperators);

    SyntaxTree::Expression* ProcessTerm(TokenIterator& iter, TokenIterator nextIter, TokenIterator endIter, bool& isPotentialEnd);

    SyntaxTree::Expression* ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const std::unordered_set<std::string>& endTokens);

    TokenIterator FindParenthesisEnd(TokenIterator iter, TokenIterator endIter);

    void ProcessExpressionOperators(std::vector<SyntaxTree::Expression*>& terms,
                                    std::vector<SyntaxTree::BinaryExpression::EOperator>& operators,
                                    const std::unordered_set<SyntaxTree::BinaryExpression::EOperator>& opsToProcess);

    SyntaxTree::StringLiteralExpression* ProcessStringExpression(TokenIterator iter);

    bool ProcessByteEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars);

    bool ProcessUnicodeEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars);

    SyntaxTree::BlockExpression* ProcessBlockExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessMemberExpression(SyntaxTree::Expression* expr, TokenIterator& iter, TokenIterator endIter);
};

#endif // SYNTAX_ANALYZER_H_
