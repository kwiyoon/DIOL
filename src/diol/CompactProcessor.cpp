#include "CompactProcessor.h"
#include "memtableController/ImmutableMemtableController.h"

/**
 * param : memtable, delayMemtables\n
 * return : memtable의 시간 범위에 맞는 delayMemtable key의 개수
 * */
long checkTimeRange(IMemtable* memtable, IMemtable* delayMemtable){
    uint64_t s = memtable->startKey;
    uint64_t l = memtable->lastKey;
    long cnt = 0;
    for(auto pair : delayMemtable->mem){
        if(pair.second >= s && pair.second <= l){
            cnt++;
        }
    }
    return cnt;
}

// Delay Memtable에서 내부적으로 시간이 겹치는 data들을 찾아 부분 compaction을 진행한다.
DelayMemtable* CompactProcessor::compaction(IMemtable* memtable, vector<IMemtable*>& delayMemtables) {
/** compaction을 진행할 DelayMemtable을 찾는다. */
IMemtable* CompactProcessor::findTargetMem(IMemtable* memtable, vector<IMemtable*>& delayMemtables) {
    ImmutableMemtableController& immutableMemtableController = ImmutableMemtableController::getInstance();
    uint64_t maxCnt = numeric_limits<uint64_t>::min();
    IMemtable* target = NULL;

    for (const auto delayMem : delayMemtables) {
        // 다른 thread가 건드는 중이거나 후보라면 패스
        if(delayMem->memTableStatus == COMPACTING
            || delayMem->memTableStatus == WAITING_FOR_COMPACT) continue;
        // ttl이 다 됐다면 M2로 즉시 변환
        if(delayMem->ttl == 0){
            immutableMemtableController.transformM1toM2(delayMem);
        }
        else{
            long cnt = checkTimeRange(memtable, delayMem);
            if(cnt > maxCnt){
                maxCnt = cnt;
                if(target != NULL) target->memTableStatus = IMMUTABLE; // 후보에서 제외
                target = delayMem;
                target->memTableStatus = WAITING_FOR_COMPACT; // 후보 등록
            }else if(cnt == maxCnt){
                if(target != NULL) target->memTableStatus = IMMUTABLE; // 후보에서 제외
                target = (target->ttl < delayMem->ttl) ? target : delayMem;
                target->memTableStatus = WAITING_FOR_COMPACT; // 후보 등록
            }
        }
    }
    return target;
}
