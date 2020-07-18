cmake_minimum_required(VERSION 3.4.3)

project(compilerapi)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

file(GLOB src "*.cpp")
add_library(compilerapi ${src})
