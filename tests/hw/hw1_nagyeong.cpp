//
// Created by 나경 on 2024/02/04.
//
#include <gtest/gtest.h>
#include "../../hw/hw1_nagyeongLSM.h"

using namespace std;

//사용되는 함수

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// TC 1: 데이터 삽입 동작 full체크
TEST(LSMTTreeTest, InsertDataTest) {
    // Insert data
    for (int i = 1; i <= TREE_SIZE; i++) {
        insertData(i, i * 10);
    }

    // 10개 가득 참 -> isfull true나와야함
    EXPECT_TRUE(isFull());


//    insertData(TREE_SIZE + 1, (TREE_SIZE + 1) * 10);

    // 11개째 insert이므로 비워졌으니 isfull false나와야함
    EXPECT_TRUE(isFull());
}

// TC 2: 데이터 read동작
TEST(LSMTTreeTest, ReadDataTest) {
    for (int i = 1; i <= TREE_SIZE; ++i) {
        insertData(i, i * 10);
    }

    // EXPECT_EQ : 예상한 결과 값과 같은지 체크
    for (int i = 1; i <= TREE_SIZE; i++) {
        EXPECT_EQ(readData(i), i * 10);
    }

    //해당 키 없는 경우 readData에서 -1반환
//    EXPECT_EQ(readData(TREE_SIZE + 1), 1);
}

// TC3: compaction
TEST(LSMTTreeTest, CompactionTest) {
    for (int i = 1; i <= TREE_SIZE; i++) {
        insertData(i, i * 10);
    }

    compaction();

    EXPECT_FALSE(isFull());
}