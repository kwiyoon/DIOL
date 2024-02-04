#include <gtest/gtest.h>
#include "../src/hw1_haena.h"

using namespace std;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST(insert,insert) {  //기본 Insert
    // given
    LSM *tree = new LSM();
    unsigned int key=1;
    int value=111;

    // when
    tree->insertData(key, value);
    cout<<"why?";
    cout << "hi "<<Mem.begin()->first << ","<< Mem.begin()->second <<" bye\n";

    for(const auto &entry : Mem){
        cout <<" 하이 "<<entry.first <<", "<<entry.second<<" 빠이 \n";
    }

    // then
    ASSERT_EQ(Mem.begin()->first, key);
    ASSERT_EQ(Mem.begin()->second, value);
}