#include "CompactionController.h"
#include "memtableController/ImmutableMemtableController.h"
//#include "stddef.h"

// TODO (new) : queue에 넣으면 ImmList에선 빼? ㅇㅇ
void CompactionController::checkTimeOut() {
    bool timeoutFound = false;
    for (const auto memtable : normalImmMemtableList_M1) {
        if (memtable->ttl == 0) {
            compactionQueue.push(memtable);
            ImmutableMemtableController::erase(normalImmMemtableList_M1, memtable);
            timeoutFound = true;
        }
    }
    /** timeoutFound = true라면, 가장 많이 겹치는 delay 찾아서 컴팩션 해야함.
        TODO (new) : ㄴ> 이건 compaction proccess 시 거기서 이루어질 것.
    */
    if (!timeoutFound) {
        // 타임아웃된 Memtable이 없을 경우 가장 delay 추정치가 적은 Memtable을 찾아 queue에 삽입
        IMemtable* minDelayMemtable = findCompactionMem();
        if(minDelayMemtable != NULL) {
            compactionQueue.push(minDelayMemtable);
            ImmutableMemtableController::erase(normalImmMemtableList_M1, minDelayMemtable);
        }
    }
}

// delay count가 가장 적은 memtable find
IMemtable* CompactionController::findCompactionMem() {
    // TODO : 수정
    int minDelay = INT_MAX;
    IMemtable* imm;
    for (const auto memtable : normalImmMemtableList_M1) {
        if (auto normalPtr = dynamic_cast<NormalMemtable*>(memtable)) {
            if (normalPtr->delayCount < minDelay) {
                minDelay = normalPtr->delayCount;
                imm = normalPtr;
            }
        }
    }
    return imm;
}