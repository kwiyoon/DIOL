#include <gtest/gtest.h>
#include "../../hw/hw1_jiyoon.cpp"

using namespace std;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(INSERT, when_not_full) {
    //given
    LSM* lsm = new LSM();
    unsigned int key = 1;
    int value = 111;

    //when
    lsm->insertData(key, value);

    //then
    EXPECT_EQ(key, memtable.begin()->first);
    EXPECT_EQ(value, memtable.begin()->second);
}

TEST(INSERT, when_full) {
    //given


    //when

    //then
}