#include "MockDisk.h"

bool MockDisk::compaction() {
    doCompaction();
}

int MockDisk::read(uint64_t key) {

    //normal SStale 뒤지기
    for (auto ss : normalSSTables) {
        // 맵에서 키 검색
        auto it = ss->rows.find(key);
        if (it != ss->rows.end()) {
            LOG_STR("(found in normalSSTable:"+ to_string(ss->sstableId)+")");
            return it->second;  // 키를 찾았으면 값 반환
        }
    }

    //없으면 delay SStale 뒤지기
    for (auto ss : delaySSTables) {
        // 맵에서 키 검색
        auto it = ss->rows.find(key);
        if (it != ss->rows.end()) {
            LOG_STR("(found in delaySStale:"+ to_string(ss->sstableId)+")");
            return it->second;  // 키를 찾았으면 값 반환
        }
    }

    //없어요
    return NULL;
}

map<uint64_t, int> MockDisk::range(uint64_t start, uint64_t end) {

//    // 로깅
//    list<string> normalSSTableIds;
//    list<string> delaySSTableIds;

    map<uint64_t, int> results;
//    bool flag;
    for (auto ss : normalSSTables) {
        for (auto it = ss->rows.lower_bound(start); it != ss->rows.end() && it->first <= end; ++it) {
//        flag = false;
            results[it->first] = it->second;
//          flag = true;
        }
//        if(flag) normalSSTableIds.push_back("("+to_string(ss->sstableId)+")");
    }
    for (auto ss : delaySSTables) {
        for (auto it = ss->rows.lower_bound(start); it != ss->rows.end() && it->first <= end; ++it) {
//        flag = false;
            results[it->first] = it->second;
//            flag = true;
        }
//        if(flag) delaySSTableIds.push_back("("+to_string(ss->sstableId)+")");
    }
    // 로깅
//    if(!normalSSTableIds.empty()) {
//        LOG_ID("found in normalSSTables ");
//        for (auto id: normalSSTableIds) LOG_ID(id);
//        LOG_STR("");
//    }
//    if(!delaySSTableIds.empty()) {
//        LOG_ID("found in delaySSTables ");
//        for(auto id : delaySSTableIds) LOG_ID(id);
//        LOG_STR("");
//    }

    return results;
}

bool MockDisk::flush(IMemtable* memtable) {
    SSTable* newSSTable = new SSTable(memtable->memtableId);

    for (const auto& entry : memtable->mem) {
        newSSTable->put(entry.first, entry.second);
    }

    newSSTable->setStartKey(newSSTable->rows.begin()->first);
    newSSTable->setLastKey(newSSTable->rows.rbegin()->first);

    if (auto normalPtr = dynamic_cast<NormalMemtable*>(memtable)) {
        newSSTable->setType(N);
        normalSSTables.push_back(newSSTable);
        doFlush(memtable->mem.size());
    } else if (auto delayPtr = dynamic_cast<DelayMemtable*>(memtable)) {
        newSSTable->setType(D);
        delaySSTables.push_back(newSSTable);
        doFlush(memtable->mem.size());
    }

    delete memtable;
    return true;
}

void MockDisk::printSSTableList() {

    LOG_STR("\n============NormalSSTable===========\n");
    for(auto table: normalSSTables){
        LOG_STR("[ " + to_string(table->sstableId)+ " ]  key: " +to_string(table->startKey)+ " ~ " + to_string(table->lastKey) + " | #cnt: " +to_string(table->rows.size()));
    }
    LOG_STR("\n============DelaySSTable===========\n");

    for(auto table: delaySSTables){
        LOG_STR("[ " + to_string(table->sstableId)+ " ]  key: " +to_string(table->startKey)+ " ~ " + to_string(table->lastKey) + " | #cnt: " +to_string(table->rows.size()));
    }

}
