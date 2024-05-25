cmake_minimum_required(VERSION 3.26.4)
project(DIOL)

set(CMAKE_CXX_STANDARD 17)
#
#add_executable(DIOL
#        hw/hw1_haena.cpp
#        hw/hw1_jiyoon.cpp
#        hw/hw1_haena.cpp
#        hw/hw_merge/hw1.cpp
#)
#
#add_subdirectory(tests)


add_executable(DIOL
        src/main.cpp
#        src/diol/CompactionController.cpp
#        src/diol/CompactionController.h
#        src/diol/CompactProcessor.cpp
#        src/diol/DBManager.cpp
#        src/diol/DelayDetector.cpp
#        src/diol/DelayDetector.h
#        src/diol/FlushController.cpp
#        src/diol/FlushController.h
#        src/diol/MockDisk.cpp
#        src/diol/SSTable.cpp
#        src/diol/memtable/DelayMemtable.cpp
#        src/diol/memtable/NormalMemtable.cpp
#        src/diol/memtableController/ActiveMemtableController.cpp
#        src/diol/memtableController/ImmutableMemtableController.cpp
)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/diol
        ${PROJECT_SOURCE_DIR}/src/diol/memtable
        ${PROJECT_SOURCE_DIR}/src/diol/memtableController
)

#add_subdirectory(tests/lib/googletest-1.14.0)