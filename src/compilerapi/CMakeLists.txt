cmake_minimum_required(VERSION 3.4.3)

project(compilerapi)

include_directories(${LLVM_INCLUDE_DIRS})

if(UNIX)
    link_libraries(stdc++fs)
endif()

add_library(compilerapi
    AssemblyGenerator.cpp
    AssemblyGenerator.h
    CHeaderPrinter.cpp
    CHeaderPrinter.h
    Compiler.cpp
    Compiler.h
    CompilerContext.cpp
    CompilerContext.h
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
    SourceGenerator.cpp
    SourceGenerator.h
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
    TokenList.cpp
    TokenList.h
    TypeInfo.cpp
    TypeInfo.h
    TypeRegistry.cpp
    TypeRegistry.h
    utils.cpp
    utils.h
)

if(WIN32)
else()
    target_compile_options(compilerapi PRIVATE -Wall -Wextra)
endif()
