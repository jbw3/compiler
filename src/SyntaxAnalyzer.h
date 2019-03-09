#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "SyntaxTree.h"
#include "Token.h"
#include <map>
#include <vector>

class SyntaxAnalyzer
{
public:
    static const std::map<std::string, SyntaxTree::BinaryExpression::EOperator>
        BINARY_EXPRESSION_OPERATORS;

    bool Process(const std::vector<Token>& tokens, SyntaxTree::SyntaxTreeNode*& syntaxTree);
};

#endif // SYNTAX_ANALYZER_H_
