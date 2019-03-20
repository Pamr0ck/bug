cmake_minimum_required(VERSION 3.13)
project(PNG_CW)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-lpng ")
add_executable(PNG_CW main.cpp)