//
// Created by 이해나 on 2/4/24.
//


#include <iostream>
#include <map>

#define MAX 10

using namespace std;

static map<unsigned int, int> Mem;
static map<unsigned int, int> Disk;

class LSM{
public:
    bool isFull();
    bool compaction();
    void insertData(unsigned int key, int value);
    int readData(int64_t key);
};



