
#include "hw1.h"

 map<unsigned int, int> Mem;
map<unsigned int, int> Disk;

//key는 timestamp 개념 (1,2,3,4 아무거나 일단 넣기)
void LSM:: insertData(unsigned int key, int value) {
    if (isFull()) {
        try {
            compaction();
        } catch (exception &e) {
            cerr << e.what() << "\n";
        }
    }
    Mem.insert(make_pair(key, value));
}

// full : 1, not full : 0
bool LSM:: isFull() {
    size_t currentSize = Mem.size() * (sizeof(unsigned int) + sizeof(int));
    return currentSize >= MEM_SIZE;
}

// value값을 return
int LSM::readData(unsigned int key) {
    auto mem_it = Mem.find(key);
    if (mem_it != Mem.end()) {
        cout << "mem 에서 find\n";
        return mem_it->second;
    } else { // key X -> disk 살피기
        diskRead(key); //빈함수
        return 0;
    }
}


// return : 성공 여부
bool LSM:: compaction() {

    if (Disk.size() >= DISK_SIZE)
        throw out_of_range("disk full");

    for (const auto &entry: Mem) {
        Disk[entry.first] = entry.second;
    }

    Mem.clear();
    return true;
}

void LSM::diskRead(unsigned int key){

    cout<<"reading Disk~ \n";

    return;

}