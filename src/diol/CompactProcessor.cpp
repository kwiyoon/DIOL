#include "CompactProcessor.h"
#include "memtableController/ImmutableMemtableController.h"

long checkTimeRange(IMemtable* memtable, IMemtable* delayMemtables){
    uint64_t s = memtable->startKey;
    uint64_t l = memtable->lastKey;
    long cnt = 0;
    for(auto pair : delayMemtables->mem){
        if(pair.second >= s && pair.second <= l){
            cnt++;
        }
    }
}

// Delay Memtable에서 내부적으로 시간이 겹치는 data들을 찾아 부분 compaction을 진행한다.
DelayMemtable* CompactProcessor::compaction(IMemtable* memtable, vector<IMemtable*>& delayMemtables) {
    ImmutableMemtableController& immutableMemtableController = ImmutableMemtableController::getInstance();
    uint64_t maxCnt = numeric_limits<uint64_t>::min();
    IMemtable* target;

    for (const auto delayMem : delayMemtables) {
        if(delayMem->ttl == 0){
            immutableMemtableController.transformM1toM2(delayMem);
        }
        else{
            long cnt = checkTimeRange(memtable, delayMem);
            if(cnt > maxCnt){
                maxCnt = cnt;
                target = delayMem;
            }else if(cnt == maxCnt){
                target = (target->ttl < delayMem->ttl) ? target : delayMem;
            }
        }
    }
}
