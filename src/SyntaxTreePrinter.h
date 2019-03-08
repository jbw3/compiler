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
    class BracePrinter
    {
    public:
        BracePrinter(SyntaxTreePrinter& printer);

        ~BracePrinter();

    private:
        SyntaxTreePrinter& printer;
    };

    unsigned int level;

    void Print(const std::string& str) const;
};

#endif // SYNTAX_TREE_PRINTER_H_
