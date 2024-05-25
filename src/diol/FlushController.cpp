#include "FlushController.h"
//#include "stddef.h"

void FlushController::checkTimeout() {
    bool timeoutFound = false;
    for (const auto memtable : normalImmMemtableList_M2) {
        if (memtable->ttl == 0) {
            flushQueue.push(memtable);
            ImmutableMemtableController::erase(normalImmMemtableList_M2, memtable);
            timeoutFound = true;
        }
    }
    for (const auto memtable : delayImmMemtableList_M2) {
        if (memtable->ttl == 0) {
            flushQueue.push(memtable);
            ImmutableMemtableController::erase(delayImmMemtableList_M2, memtable);
            timeoutFound = true;
        }
    }
    if (!timeoutFound) {
        // 타임아웃된 Memtable이 없을 경우 access count가 가장 적은 Memtable을 찾아 queue에 삽입
        IMemtable* normalMem = findFlushMem(normalImmMemtableList_M2);
        IMemtable* delayMem = findFlushMem(delayImmMemtableList_M2);
        if(normalMem != NULL) {
            flushQueue.push(normalMem);
            ImmutableMemtableController::erase(normalImmMemtableList_M2, normalMem);
        }
        if(delayMem != NULL) {
            flushQueue.push(delayMem);
            ImmutableMemtableController::erase(delayImmMemtableList_M2, normalMem);
        }
    }
}

// access count가 가장 적은 memtable find
IMemtable* FlushController::findFlushMem(vector<IMemtable*>& v) {
    int minAccess = INT_MAX;
    IMemtable* imm;

    for (const auto memtable : v) {
        if (memtable->access < minAccess) {
            minAccess = memtable->access;
            imm = memtable;
        }
    }
    return imm;
}
