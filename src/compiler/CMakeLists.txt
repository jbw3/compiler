cmake_minimum_required(VERSION 3.4.3)

project(compiler)

include_directories(../compilerapi)
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(wip main.cpp)

target_link_libraries(wip compilerapi ${llvm_libs})

if(WIN32)
else()
    target_compile_options(compilerapi PRIVATE -Wall -Wextra)
endif()
