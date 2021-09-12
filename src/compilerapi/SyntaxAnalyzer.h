#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "ErrorLogger.h"
#include "SyntaxTree.h"
#include "Token.h"
#include "TokenList.h"
#include <stack>
#include <unordered_set>
#include <vector>

class CompilerContext;

class SyntaxAnalyzer
{
public:
    typedef TokenList::const_iterator TokenIterator;

    SyntaxAnalyzer(CompilerContext& compilerContext, ErrorLogger& logger);

    bool Process(SyntaxTree::Modules* syntaxTree);

private:
    struct UnaryOpData
    {
        const Token* token;
        SyntaxTree::UnaryExpression::EOperator op;
    };

    struct BinaryOpData
    {
        const Token* token;
        SyntaxTree::BinaryExpression::EOperator op;
    };

    CompilerContext& compilerContext;
    ErrorLogger& logger;

    bool ProcessModule(unsigned fileId, const TokenList& tokens, SyntaxTree::ModuleDefinition*& syntaxTree);

    bool EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIteratorCheckType(TokenIterator& iter, const TokenIterator& endIter, Token::EType expectedTokenType, const char* errorMsg = nullptr);

    bool ProcessType(TokenIterator& iter, const TokenIterator& endIter,
                     SyntaxTree::Expression*& typeExpression,
                     Token::EType endTokenType1, Token::EType endTokenType2 = Token::eInvalid);

    SyntaxTree::ExternFunctionDeclaration* ProcessExternFunction(TokenIterator& iter,
                                                                 TokenIterator endIter);

    SyntaxTree::FunctionDefinition* ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter);

    SyntaxTree::FunctionDeclaration* ProcessFunctionDeclaration(TokenIterator& iter,
                                                                TokenIterator endIter,
                                                                Token::EType endTokenType);

    bool ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                           SyntaxTree::Parameters& parameters);

    SyntaxTree::StructDefinition* ProcessStructDefinition(TokenIterator& iter,
                                                          TokenIterator endIter,
                                                          unsigned fileId);

    bool IsStructInitialization(TokenIterator iter, TokenIterator endIter);

    SyntaxTree::StructInitializationExpression* ProcessStructInitialization(TokenIterator& iter,
                                                                            TokenIterator endIter);

    SyntaxTree::ConstantDeclaration* ProcessConstantDeclaration(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::VariableDeclaration* ProcessVariableDeclaration(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::WhileLoop* ProcessWhileLoop(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::ForLoop* ProcessForLoop(TokenIterator& iter, TokenIterator endIter);

    TokenIterator FindStatementEnd(TokenIterator iter, TokenIterator endIter);

    SyntaxTree::Expression* AddUnaryExpressions(SyntaxTree::Expression* baseExpr,
                                                std::stack<UnaryOpData>& unaryOperators);

    SyntaxTree::Expression* ProcessTerm(TokenIterator& iter, TokenIterator nextIter, TokenIterator endIter, bool& isPotentialEnd);

    SyntaxTree::Expression* ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              Token::EType endTokenType1 = Token::eInvalid,
                                              Token::EType endTokenType2 = Token::eInvalid,
                                              Token::EType endTokenType3 = Token::eInvalid);

    TokenIterator FindParenthesisEnd(TokenIterator iter, TokenIterator endIter);

    void ProcessExpressionOperators(std::vector<SyntaxTree::Expression*>& terms,
                                    std::vector<BinaryOpData>& operators,
                                    const std::unordered_set<SyntaxTree::BinaryExpression::EOperator>& opsToProcess);

    SyntaxTree::StringLiteralExpression* ProcessStringExpression(TokenIterator iter);

    bool ProcessByteEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars);

    bool ProcessUnicodeEscapeSequence(const TokenIterator& iter, size_t& idx, std::vector<char>& chars);

    SyntaxTree::BlockExpression* ProcessBlockExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessPostTerm(SyntaxTree::Expression* expr, TokenIterator& iter, TokenIterator endIter);
};

#endif // SYNTAX_ANALYZER_H_
