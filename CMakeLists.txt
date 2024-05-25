cmake_minimum_required(VERSION 3.26.4)
project(DIOL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Threads)
add_executable(DIOL
        src/main.cpp
)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/diol
        ${PROJECT_SOURCE_DIR}/src/diol/memtable
        ${PROJECT_SOURCE_DIR}/src/diol/memtableController
)

target_link_libraries (DIOL ${CMAKE_THREAD_LIBS_INIT})
