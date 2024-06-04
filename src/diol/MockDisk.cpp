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
//        flag = false;
        if(ss->startKey < start || ss->lastKey > end)
            continue;

        auto itStart = ss->rows.lower_bound(start); // start 이상의 첫 번째 요소를 찾음
        auto itEnd = ss->rows.upper_bound(end);     // end 이하의 마지막 요소의 다음 요소를 찾음

        for (auto it = itStart; it != itEnd; ++it) {
            results[it->first] = it->second;
//          flag = true;
        }
//        if(flag) normalSSTableIds.push_back("("+to_string(ss->sstableId)+")");
    }
    for (auto ss : delaySSTables) {
//        flag = false;
        if(ss->startKey < start || ss->lastKey > end)
            continue;
        
        auto itStart = ss->rows.lower_bound(start); // start 이상의 첫 번째 요소를 찾음
        auto itEnd = ss->rows.upper_bound(end);     // end 이하의 마지막 요소의 다음 요소를 찾음

        for (auto it = itStart; it != itEnd; ++it) {
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

bool MockDisk::flush(IMemtable* memtable, Type t) {
//    std::unique_lock<std::mutex> lock(memtable->immMutex);
//    cout << "MockDisk::flush - id : " << memtable->memtableId <<endl;

    SSTable* newSSTable = new SSTable(memtable->memtableId);

    try {
        for (const auto& entry : memtable->mem) {
            newSSTable->put(entry.first, entry.second);
        }
    } catch (const std::exception& e) {
            std::cerr << "Exception caught in main: " << e.what() << std::endl;
    }


    newSSTable->setStartKey(newSSTable->rows.begin()->first);
    newSSTable->setLastKey(newSSTable->rows.rbegin()->first);
//    cout<<"newSSTable 범위 : "<<newSSTable->startKey << ", "<<newSSTable->lastKey << endl;

    newSSTable->setType(t);
    if (t == N) {
//        cout <<"normalSSTables size : "<< normalSSTables.size() <<"->";
        normalSSTables.push_back(newSSTable);
//        cout << normalSSTables.size() <<endl;
        doFlush(memtable->mem.size());
        return true;

    } else if (t == D) {
//        cout <<"delaySSTables size : "<< delaySSTables.size() <<"->";
        delaySSTables.push_back(newSSTable);
//        cout << delaySSTables.size() <<endl;
        doFlush(memtable->mem.size());
        return true;

    }

    return false;
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
