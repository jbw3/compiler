cmake_minimum_required(VERSION 3.4.3)

enable_testing()

project(wip)

add_subdirectory(compilerapi)
add_subdirectory(compiler)
add_subdirectory(unittests)

add_test(NAME tests COMMAND unittests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
