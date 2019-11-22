#ifndef SYNTAX_TREE_PRINTER_H_
#define SYNTAX_TREE_PRINTER_H_

#include "SyntaxTreeVisitor.h"
#include "SyntaxTree.h"
#include <string>
#include <vector>

class SyntaxTreePrinter : public SyntaxTreeVisitor
{
public:
    static const std::string NODE_TYPE_PROPERTY;

    SyntaxTreePrinter(const std::string& outFilename);

    ~SyntaxTreePrinter();

    void Visit(SyntaxTree::UnaryExpression* unaryExpression) override;

    void Visit(SyntaxTree::BinaryExpression* binaryExpression) override;

    void Visit(SyntaxTree::Assignment* assignment) override;

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

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
    bool firstItem;

    void PrintVariableDefinition(const SyntaxTree::VariableDefinition* variableDefinition);

    void PrintExpressions(const std::string& attributeName, const SyntaxTree::Expressions& expressions);

    void PrintProperty(const std::string& name, const std::string& value);

    void PrintProperty(const std::string& name, SyntaxTree::SyntaxTreeNode* value);

    void Print(const std::string& str);
};

#endif // SYNTAX_TREE_PRINTER_H_
