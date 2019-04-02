#ifndef SYNTAX_TREE_PRINTER_H_
#define SYNTAX_TREE_PRINTER_H_

#include "SyntaxTreeVisitor.h"
#include <string>

namespace SyntaxTree
{
class VariableDefinition;
}

class SyntaxTreePrinter : public SyntaxTreeVisitor
{
public:
    SyntaxTreePrinter(const std::string& outFilename);

    ~SyntaxTreePrinter();

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

private:
    class BracePrinter
    {
    public:
        BracePrinter(SyntaxTreePrinter& printer, std::string start, std::string end);

        ~BracePrinter();

    private:
        SyntaxTreePrinter& printer;
        std::string startStr;
        std::string endStr;
    };

    std::ostream* os;
    unsigned int level;

    void PrintVariableDefinition(const SyntaxTree::VariableDefinition* variableDefinition);

    void Print(const std::string& str) const;
};

#endif // SYNTAX_TREE_PRINTER_H_
