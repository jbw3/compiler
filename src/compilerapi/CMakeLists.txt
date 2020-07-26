cmake_minimum_required(VERSION 3.4.3)

project(compilerapi)

include_directories(${LLVM_INCLUDE_DIRS})

add_library(compilerapi
    AssemblyGenerator.cpp
    AssemblyGenerator.h
    CHeaderPrinter.cpp
    CHeaderPrinter.h
    Compiler.cpp
    Compiler.h
    Config.cpp
    Config.h
    ErrorLogger.cpp
    ErrorLogger.h
    keywords.h
    LexicalAnalyzer.cpp
    LexicalAnalyzer.h
    LlvmIrGenerator.cpp
    LlvmIrGenerator.h
    LlvmIrOptimizer.cpp
    LlvmIrOptimizer.h
    SemanticAnalyzer.cpp
    SemanticAnalyzer.h
    Stopwatch.cpp
    Stopwatch.h
    SymbolTable.cpp
    SymbolTable.h
    SyntaxAnalyzer.cpp
    SyntaxAnalyzer.h
    SyntaxTree.cpp
    SyntaxTree.h
    SyntaxTreePrinter.cpp
    SyntaxTreePrinter.h
    SyntaxTreeVisitor.h
    Token.cpp
    Token.h
    TypeInfo.cpp
    TypeInfo.h
    utils.cpp
    utils.h
)
