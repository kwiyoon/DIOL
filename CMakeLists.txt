cmake_minimum_required(VERSION 3.26.4)
project(DIOL)

set(CMAKE_CXX_STANDARD 17)

add_executable(DIOL
        hw/hw1_haena.cpp
        hw/hw1_jiyoon.cpp
        hw/hw1_haena.cpp
        hw/hw_merge/hw1.cpp
)

add_subdirectory(tests)
add_subdirectory(tests/lib/googletest-1.14.0)