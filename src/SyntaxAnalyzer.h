#ifndef SYNTAX_ANALYZER_H_
#define SYNTAX_ANALYZER_H_

#include "Token.h"
#include <vector>

namespace SyntaxTree
{
class SyntaxTreeNode;
}

class SyntaxAnalyzer
{
public:
    SyntaxTree::SyntaxTreeNode* Process(const std::vector<Token>& tokens);
};

#endif // SYNTAX_ANALYZER_H_
