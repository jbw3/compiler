#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "Token.h"
#include <unordered_set>
#include <vector>

namespace SyntaxTree
{
class SyntaxTreeNode;
}

class SyntaxAnalyzer
{
public:
    static const std::unordered_set<std::string> BINARY_EXPRESSION_OPERATORS;

    SyntaxTree::SyntaxTreeNode* Process(const std::vector<Token>& tokens);
};

#endif // SYNTAX_ANALYZER_H_
