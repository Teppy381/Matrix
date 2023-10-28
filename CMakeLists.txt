cmake_minimum_required(VERSION 3.5.0)
project(matrices VERSION 0.1.0 LANGUAGES C CXX)


include(CTest)
add_subdirectory(external)
add_subdirectory(matrix_test)

add_executable(matrices main.cpp matrices.hpp)

