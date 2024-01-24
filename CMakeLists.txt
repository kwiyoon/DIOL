cmake_minimum_required(VERSION 3.27)
project(DIOL)

set(CMAKE_CXX_STANDARD 17)

add_executable(DIOL src/main_example.cpp
        src/MemoryManager.cpp
        src/memory/memtable/Memtable.cpp
        src/memory/MemtableController.cpp
        src/memory/CompactionController.cpp
        src/memory/CompactionProcessor.cpp
        src/memory/FlushController.cpp
        src/memory/memtable/ActiveMemtable.cpp
        src/memory/DelayDetector.cpp
        src/memory/memtable/ImmutableMemtable.cpp)

add_subdirectory(tests)
add_subdirectory(tests/lib/googletest-1.14.0)