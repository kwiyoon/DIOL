include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(hw1_haena_test
        hw1_haena_test.cpp
        ../src/hw1_haena.cpp
)

target_link_libraries(hw1_haena_test gtest gtest_main)