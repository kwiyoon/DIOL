cmake_minimum_required(VERSION 3.27)
project(DIOL)

set(CMAKE_CXX_STANDARD 17)

add_executable(DIOL src/main_example.cpp)

add_subdirectory(tests)
add_subdirectory(tests/lib/googletest-1.14.0)