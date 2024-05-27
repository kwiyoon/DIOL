#include "IMemtable.h"

NormalMemtable::NormalMemtable(int id) {
    this->state = M0;
    this->startKey = 0;
    this->lastKey = static_cast<uint64_t>(-1);
    this->memtableId = id;
    this->memTableStatus = WORKING;
}

void NormalMemtable::setDelayCount(int cnt) {
    delayCount = cnt;
}
