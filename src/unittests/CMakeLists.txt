cmake_minimum_required(VERSION 3.4.3)

project(unittests)

find_package(LLVM REQUIRED CONFIG)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

add_executable(unittests
    CompilerTests.cpp
    LexicalAnalyzerTests.cpp
    main.cpp
    TestClass.cpp
    UtilsTests.cpp
)

execute_process(COMMAND llvm-config-10 --libs all
                OUTPUT_VARIABLE llvm_libs
                OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS ${llvm_libs})
target_link_libraries(unittests compilerapi ${llvm_libs})
