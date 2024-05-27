#include "FlushController.h"
//#include "stddef.h"

// TODO : M2 리스트의 80% 이상이 찼을 경우 실행되는 메소드
void FlushController::checkTimeout(Type t) {
    cout<<"FlushController::checkTimeout()\n";
    ImmutableMemtableController& imm = ImmutableMemtableController::getInstance();
    bool timeoutFound = false;
    if(t==N) {
        for (const auto memtable: imm.normalImmMemtableList_M2) {
            if (memtable->ttl <= 0) {
                cout << "flushQueue.push\n";
                imm.flushQueue.push(memtable);
                imm.erase(imm.normalImmMemtableList_M2, memtable);
                timeoutFound = true;
            }
        }
    }
    if(t == D) {
        for (const auto memtable: imm.delayImmMemtableList_M2) {
            if (memtable->ttl <= 0) {
                cout << "flushQueue.push\n";
                imm.flushQueue.push(memtable);
                imm.erase(imm.delayImmMemtableList_M2, memtable);
                timeoutFound = true;
            }
        }
    }
    if (!timeoutFound) {
        // 타임아웃된 Memtable이 없을 경우 access count가 가장 적은 Memtable을 찾아 queue에 삽입
        IMemtable* normalMem = findMemtableWithMinAccess(imm.normalImmMemtableList_M2);
        IMemtable* delayMem = findMemtableWithMinAccess(imm.delayImmMemtableList_M2);
        if(normalMem != NULL) {
            cout<<"flushQueue.push\n";
            imm.flushQueue.push(normalMem);
            imm.erase(imm.normalImmMemtableList_M2, normalMem);
        }
        if(delayMem != NULL) {
            cout<<"flushQueue.push\n";
            imm.flushQueue.push(delayMem);
            imm.erase(imm.delayImmMemtableList_M2, normalMem);
        }
    }
}

// access count가 가장 적은 memtable find
IMemtable* FlushController::findMemtableWithMinAccess(vector<IMemtable*>& v) {
    int minAccess = INT_MAX;
    IMemtable* imm = NULL;
    for (const auto memtable: v) {
        if (memtable->access < minAccess) {
            minAccess = memtable->access;
            imm = memtable;
        }
    }
    return imm;
}
