include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(test_example
        test_example.cpp
        ../src/main_example.cpp
)

target_link_libraries(test_example gtest gtest_main)