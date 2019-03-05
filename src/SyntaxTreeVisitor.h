#ifndef SYNTAX_TREE_VISITOR_H_
#define SYNTAX_TREE_VISITOR_H_

#include "SyntaxTree.h"

class SyntaxTreeVisitor
{
public:
    virtual void Visit(const SyntaxTree::Assignment* assignment) = 0;

    virtual void Visit(const SyntaxTree::BinaryExpression* assignment) = 0;

    virtual void Visit(const SyntaxTree::NumericExpression* numericExpression) = 0;

    virtual void Visit(const SyntaxTree::Variable* variable) = 0;
};

#endif // SYNTAX_TREE_VISITOR_H_
