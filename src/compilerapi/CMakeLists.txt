cmake_minimum_required(VERSION 3.4.3)

project(compilerapi)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

add_library(compilerapi
    AssemblyGenerator.cpp
    CHeaderPrinter.cpp
    Compiler.cpp
    Config.cpp
    ErrorLogger.cpp
    LexicalAnalyzer.cpp
    LlvmIrGenerator.cpp
    LlvmIrOptimizer.cpp
    SemanticAnalyzer.cpp
    Stopwatch.cpp
    SymbolTable.cpp
    SyntaxAnalyzer.cpp
    SyntaxTree.cpp
    SyntaxTreePrinter.cpp
    Token.cpp
    TypeInfo.cpp
    utils.cpp
)
