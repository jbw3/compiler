cmake_minimum_required(VERSION 3.4.3)

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
    SourceGeneratorTests.cpp
    SourceGeneratorTests.h
    TestClass.cpp
    TestClass.h
    UtilsTests.cpp
    UtilsTests.h
)

target_link_libraries(unittests compilerapi ${llvm_libs})

if(WIN32)
else()
    target_compile_options(unittests PRIVATE -Wall -Wextra)
endif()
