#include "IMemtable.h"
#include <iostream>
NormalMemtable::NormalMemtable(int id) {
    this->state = M0;
    this->startKey = 0;
    this->lastKey = static_cast<uint64_t>(-1);
    this->memtableId = id;
    this->memTableStatus = WORKING;
    this->memtableSize = 8 * 1024 * 1024;
}

void NormalMemtable::setDelayCount(int cnt) {
    delayCount = cnt;
}
