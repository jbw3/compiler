cmake_minimum_required(VERSION 3.5.0)

project(unittests)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(unittests
    CompilerTests.cpp
    CompilerTests.h
    LexicalAnalyzerTests.cpp
    LexicalAnalyzerTests.h
    main.cpp
    ROStringTests.cpp
    ROStringTests.h
    SemanticAnalyzerTests.cpp
    SemanticAnalyzerTests.h
    SourceGeneratorTests.cpp
    SourceGeneratorTests.h
    StartEndTokenFinderTests.cpp
    StartEndTokenFinderTests.h
    SyntaxAnalyzerTests.cpp
    SyntaxAnalyzerTests.h
    TestClass.cpp
    TestClass.h
    UtilsTests.cpp
    UtilsTests.h
)

target_link_libraries(unittests compilerapi ${llvm_libs})

if(CODE_COVERAGE)
    target_compile_options(unittests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(unittests PRIVATE -fprofile-instr-generate)
endif()

if(WIN32)
else()
    target_compile_options(unittests PRIVATE -Wall -Wextra)
endif()
