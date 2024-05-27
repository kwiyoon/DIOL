cmake_minimum_required(VERSION 3.26.4)
project(DIOL)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Threads package
find_package(Threads REQUIRED)

# Include directories
include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/diol
        ${PROJECT_SOURCE_DIR}/src/diol/memtable
        ${PROJECT_SOURCE_DIR}/src/diol/memtableController
)

# Add executables
add_executable(DIOL

        src/main.cpp
        src/diol/FlushController.cpp
        src/diol/memtable/DelayMemtable.cpp
        src/diol/memtable/IMemtable.cpp
        src/diol/memtable/NormalMemtable.cpp
        src/diol/memtableController/ActiveMemtableController.cpp
        src/diol/memtableController/ImmutableMemtableController.cpp
        src/diol/CompactionController.cpp
        src/diol/CompactProcessor.cpp
        src/diol/DBManager.cpp
        src/diol/DelayDetector.cpp
        src/diol/MockDisk.cpp
        src/diol/SSTable.cpp
        src/test/VectorTest.cpp
)



# Link the pthread library
target_link_libraries(DIOL
        ${CMAKE_THREAD_LIBS_INIT}
)
