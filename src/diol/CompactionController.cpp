#include "CompactionController.h"
#include "memtableController/ImmutableMemtableController.h"

void CompactionController::checkTimeOut() {
    cout<<"CompactionController::checkTimeOut()\n";
    ImmutableMemtableController& immController = ImmutableMemtableController::getInstance();
    bool timeoutFound = false;
    if(!immController.normalImmMemtableList_M1.empty()){
        for (const auto memtable : immController.normalImmMemtableList_M1) {
            if (memtable->ttl == 0) {
                cout<<"compactionQueue.push - timeout\n";
                immController.compactionQueue.push(memtable);
                immController.erase(immController.normalImmMemtableList_M1, memtable);
                timeoutFound = true;
            }
        }
    }

    if (!timeoutFound) {
        // 타임아웃된 Memtable이 없을 경우 가장 delay 추정치가 적은 Memtable을 찾아 queue에 삽입
        IMemtable* minDelayMemtable = findCompactionMem();
        if(minDelayMemtable != NULL) {
            cout<<"compactionQueue.push - least delay\n";
            cout<<"immController.compactionQueue.size(): "<<immController.compactionQueue.size()<<endl;
            immController.compactionQueue.push(minDelayMemtable);
            cout<<"immController.compactionQueue.size(): "<<immController.compactionQueue.size()<<endl;

            immController.erase(immController.normalImmMemtableList_M1, minDelayMemtable);
        }
    }
}

// delay count가 가장 적은 memtable find
IMemtable* CompactionController::findCompactionMem() {
    ImmutableMemtableController& immContoller = ImmutableMemtableController::getInstance();

    int minDelay = INT_MAX;
    IMemtable* imm = NULL;
    if(!immContoller.normalImmMemtableList_M1.empty()) {
        for (const auto memtable: immContoller.normalImmMemtableList_M1) {
            if (auto normalPtr = dynamic_cast<NormalMemtable *>(memtable)) {
                if (normalPtr->delayCount < minDelay) {
                    minDelay = normalPtr->delayCount;
                    imm = normalPtr;
                }
            }
        }
    }
    return imm;
}