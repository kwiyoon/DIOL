/*
 1. 메모리는 STL (linked_list)
*/
#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

const size_t MEM_SIZE = 80;
const size_t DISK_SIZE = 400;
static map<unsigned int, int> memtable;
static map<unsigned int, int> disk;

class LSM {
public:
    //key는 timestamp개념 (1,2,3,4 아무거나 일단 넣기)
    void insertData(unsigned int key, int value) {
        if (isFull()) {
            try {
                compaction();
            } catch (exception &e) {
                cerr << e.what() << "\n";
            }
        }
        memtable.emplace(key, value);
    }

// full : 1, not full : 0
    bool isFull() {
        size_t currentSize = memtable.size() * (sizeof(unsigned int) + sizeof(int));
        return currentSize >= MEM_SIZE;
    }

// value값을 return
    int readData(unsigned int key) {
        auto mem_it = memtable.find(key);
        if (mem_it != memtable.end()) {
            cout << "mem 에서 find\n";
            return mem_it->second;
        } else { // key X -> disk 살피기
            auto disk_it = disk.find(key);
            if (disk_it != disk.end()) {
                cout << "disk 에서 find\n";
                return disk_it->second;
            } else {
                cerr << "data 없음!!\n";
            }
        }
    }


// return : 성공 여부
    bool compaction() {

        if (disk.size() >= DISK_SIZE)
            throw out_of_range("disk full");

        for (const auto &entry: memtable) {
            disk[entry.first] = entry.second;
        }

        return true;
    }
};