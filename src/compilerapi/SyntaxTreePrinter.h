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

    void Visit(SyntaxTree::WhileLoop* whileLoop) override;

    void Visit(SyntaxTree::ExternFunctionDeclaration* externFunctionDeclaration) override;

    void Visit(SyntaxTree::FunctionDefinition* functionDefinition) override;

    void Visit(SyntaxTree::TypeDefinition* typeDefinition) override;

    void Visit(SyntaxTree::TypeInitializationExpression* typeInitializationExpression) override;

    void Visit(SyntaxTree::ModuleDefinition* moduleDefinition) override;

    void Visit(SyntaxTree::NumericExpression* numericExpression) override;

    void Visit(SyntaxTree::UnitTypeLiteralExpression* unitTypeLiteralExpression) override;

    void Visit(SyntaxTree::BoolLiteralExpression* boolLiteralExpression) override;

    void Visit(SyntaxTree::StringLiteralExpression* stringLiteralExpression) override;

    void Visit(SyntaxTree::VariableExpression* variableExpression) override;

    void Visit(SyntaxTree::BlockExpression* blockExpression) override;

    void Visit(SyntaxTree::FunctionExpression* functionExpression) override;

    void Visit(SyntaxTree::MemberExpression* memberExpression) override;

    void Visit(SyntaxTree::BranchExpression* branchExpression) override;

    void Visit(SyntaxTree::VariableDeclaration* variableDeclaration) override;

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

    void PrintParameter(const SyntaxTree::Parameter* parameter);

    void PrintFunctionDeclaration(const SyntaxTree::FunctionDeclaration* declaration);

    void PrintMemberDefinition(const SyntaxTree::MemberDefinition* member);

    void PrintMemberInitialization(const SyntaxTree::MemberInitialization* memberInitialization);

    void PrintProperty(const std::string& name, const std::string& value);

    void PrintProperty(const std::string& name, SyntaxTree::SyntaxTreeNode* value);

    template<typename T>
    void PrintProperty(const std::string& name, T value, std::function<void (T)> printValue)
    {
        if (firstItem)
        {
            firstItem = false;
        }
        else
        {
            Print(",\n");
        }

        Print("\"");
        Print(name);
        Print("\":\n");
        printValue(value);
    }

    template<typename T>
    void PrintProperty(const std::string& name, const std::vector<T>& values, std::function<void (T)> printValue)
    {
        if (firstItem)
        {
            firstItem = false;
        }
        else
        {
            Print(",\n");
        }

        Print("\"");
        Print(name);
        Print("\": ");

        size_t numExpressions = values.size();
        if (numExpressions == 0)
        {
            Print("[]");
        }
        else
        {
            BracePrinter printer3(*this, "[", "]");

            printValue(values[0]);
            for (size_t i = 1; i < numExpressions; ++i)
            {
                Print(",\n");
                printValue(values[i]);
            }
        }
    }

    template<typename T>
    void PrintProperty(const std::string& name, const std::vector<T>& values)
    {
        std::function<void (T)> fun = [this](T v){ v->Accept(this); };
        PrintProperty(name, values, fun);
    }

    void Print(const std::string& str);
};

#endif // SYNTAX_TREE_PRINTER_H_
