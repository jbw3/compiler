cmake_minimum_required(VERSION 3.5.0)

project(compiler)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(wip main.cpp)

target_link_libraries(wip compilerapi ${llvm_libs})

if(WIN32)
else()
    target_compile_options(wip PRIVATE -Wall -Wextra)
endif()
