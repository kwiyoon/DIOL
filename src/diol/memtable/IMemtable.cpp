#include "IMemtable.h"

size_t IMemtable::getSize() {
    // 현재 map size
    size_t currentMapSize = mem.size() * (sizeof(uint64_t) + sizeof(int));
    // 객체 크기
    size_t instanceSize = sizeof(*this);
    return instanceSize + currentMapSize;
}

bool IMemtable::isFull(){
    size_t incomingDataSize = sizeof(uint64_t) + sizeof(int);
    return (getSize() + incomingDataSize) >= (memtableSize * 0.8);
}

bool IMemtable::put(uint64_t key, int value){
    return mem.insert({key, value}).second;
}
bool IMemtable::setState(State state){
    this->state = state;
    return true;
}

bool IMemtable::setStartKey(uint64_t key){
    this->startKey = key;
    return true;
}
bool IMemtable::setLastKey(uint64_t key){
    this->lastKey=key;
    return true;
}

// TODO (new) : TTL 정하기
void IMemtable::initTTL() {
    if(this->state == M1){
        this->ttl = INIT_TTL/2; // 10
    }else if(this->state == M2) {
        this->ttl = INIT_TTL;   // 20
    }
}

void IMemtable::initM1() {
    this->memTableStatus = IMMUTABLE;
    this->setState(M1);
    this->setStartKey(this->mem.begin()->first);
    this->setLastKey(this->mem.rbegin()->first);
    this->initTTL();
}

void IMemtable::initM2() {
    this->memTableStatus = COMPACTED;
    this->setState(M2);
    this->initTTL();
}

void IMemtable::increaseAccessCount(int cnt) {
    access+=cnt;
}

