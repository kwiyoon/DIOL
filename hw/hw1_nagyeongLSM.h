//
// Created by 나경 on 2024/02/04.
//

#ifndef DIOL_HW1_NAGYEONGLSM_H
#define DIOL_HW1_NAGYEONGLSM_H
static std::map<int, int> lsmTree;

int insertData(unsigned int key, int value);
bool isFull();
int readData(int key);
bool compaction();
const int TREE_SIZE = 10;

#endif //DIOL_HW1_NAGYEONGLSM_H
