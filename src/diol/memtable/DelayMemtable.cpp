#include "IMemtable.h"
#include <iostream>

DelayMemtable::DelayMemtable(int id) {
    this->state = M0;
    this->startKey = 0;
    this->lastKey = static_cast<uint64_t>(-1);
    this->memtableId = id;
    this->memTableStatus = WORKING;
//    this->memtableSize = 1024 * 1024;
    this->memtableSize = 512 * 1024;
}

bool DelayMemtable::isFull(){
    size_t incomingDataSize = sizeof(uint64_t) + sizeof(int);
    return (getSize() + incomingDataSize) >= (memtableSize);
}