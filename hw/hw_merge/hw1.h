

#ifndef DIOL_HW1_H
#define DIOL_HW1_H

#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

extern map<unsigned int, int> Mem;
extern map<unsigned int, int> Disk;

const size_t MEM_SIZE = 80;
const size_t DISK_SIZE = 400;

class LSM{

public:
    bool isFull();
    bool compaction();
    void insertData(unsigned int key, int value);
    int readData(unsigned int key);
    void diskRead(unsigned int key);

};


#endif //DIOL_HW1_H
