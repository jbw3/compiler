#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "SyntaxTree.h"
#include "Token.h"
#include <map>
#include <unordered_set>
#include <vector>

class SyntaxAnalyzer
{
public:
    typedef std::vector<Token> TokenSequence;
    typedef TokenSequence::const_iterator TokenIterator;

    static const std::string FUNCTION_KEYWORD;

    static const std::map<std::string, SyntaxTree::BinaryExpression::EOperator> BINARY_EXPRESSION_OPERATORS;

    static const std::map<std::string, SyntaxTree::Expression::EType> TYPES;

    bool Process(const TokenSequence& tokens, SyntaxTree::SyntaxTreeNode*& syntaxTree);

private:
    bool SkipNewlines(TokenIterator& iter, TokenIterator endIter);

    SyntaxTree::FunctionDefinition* ProcessFunctionDefinition(TokenIterator& iter,
                                                              TokenIterator endIter);

    bool ProcessParameters(TokenIterator& iter, TokenIterator endIter,
                           std::vector<SyntaxTree::VariableDefinition*>& parameters);

    SyntaxTree::Expression* ProcessExpression(TokenIterator& iter, TokenIterator endIter,
                                              const std::unordered_set<std::string>& endTokens);
};

#endif // SYNTAX_ANALYZER_H_
