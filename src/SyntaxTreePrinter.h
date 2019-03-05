#ifndef SYNTAX_TREE_PRINTER_H_
#define SYNTAX_TREE_PRINTER_H_

#include "SyntaxTreeVisitor.h"

class SyntaxTreePrinter : public SyntaxTreeVisitor
{
public:
    SyntaxTreePrinter();

    void Visit(const SyntaxTree::Assignment* assignment) override;

    void Visit(const SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(const SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(const SyntaxTree::Variable* variable) override;

private:
    unsigned int level;
};

#endif // SYNTAX_TREE_PRINTER_H_
