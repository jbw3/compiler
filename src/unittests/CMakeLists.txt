cmake_minimum_required(VERSION 3.4.3)

project(unittests)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(unittests
    CompilerTests.cpp
    LexicalAnalyzerTests.cpp
    main.cpp
    TestClass.cpp
    UtilsTests.cpp
)

target_link_libraries(unittests compilerapi ${llvm_libs})
