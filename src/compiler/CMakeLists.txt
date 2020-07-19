cmake_minimum_required(VERSION 3.4.3)

project(compiler)

find_package(LLVM REQUIRED CONFIG)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

add_executable(wip main.cpp)

execute_process(COMMAND llvm-config-10 --libs all
                OUTPUT_VARIABLE llvm_libs
                OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS ${llvm_libs})
target_link_libraries(wip compilerapi ${llvm_libs})
