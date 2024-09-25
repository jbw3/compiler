#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "SyntaxTree.h"
#include "Token.h"
#include "TokenList.h"
#include <stack>
#include <unordered_set>
#include <vector>

class CompilerContext;
class ErrorLogger;

class SyntaxAnalyzer
{
public:
    typedef TokenList::const_iterator TokenIterator;

    SyntaxAnalyzer(CompilerContext& compilerContext);

    bool Process(SyntaxTree::Modules* syntaxTree);

    bool ProcessModule(unsigned fileId, const TokenList& tokens, SyntaxTree::ModuleDefinition*& syntaxTree);

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
    unsigned currentFileId;

    bool EndIteratorCheck(const TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIterator(TokenIterator& iter, const TokenIterator& endIter, const char* errorMsg = nullptr);

    bool IncrementIteratorCheckType(TokenIterator& iter, const TokenIterator& endIter, Token::EType expectedTokenType, const char* errorMsg = nullptr);

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
                                                          TokenIterator endIter);

    SyntaxTree::StructDefinitionExpression* ProcessStructDefinitionExpression(
        TokenIterator& iter,
        TokenIterator endIter
    );

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

    SyntaxTree::Expression* ProcessTerm(TokenIterator& iter, TokenIterator endIter,
                                        bool& isPotentialEnd,
                                        Token::EType endTokenType1 = Token::eInvalid,
                                        Token::EType endTokenType2 = Token::eInvalid,
                                        Token::EType endTokenType3 = Token::eInvalid);

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

    SyntaxTree::BranchExpression* ProcessBranchExpression(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::Expression* ProcessPostTerm(SyntaxTree::Expression* expr, TokenIterator& iter, TokenIterator endIter);
};

#endif // SYNTAX_ANALYZER_H_
