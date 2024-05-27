#include "IMemtable.h"
#include <iostream>

DelayMemtable::DelayMemtable(int id) {
    this->state = M0;
    this->startKey = 0;
    this->lastKey = static_cast<uint64_t>(-1);
    this->memtableId = id;
    this->memTableStatus = WORKING;
    std::cout<<"memtableId: "<<memtableId<<"\n";
}
