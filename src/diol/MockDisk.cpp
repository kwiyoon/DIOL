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
            cout<<"(found in normalSSTable:"<<ss->sstableId<<")";
            return it->second;  // 키를 찾았으면 값 반환
        }
    }

    //없으면 delay SStale 뒤지기
    for (auto ss : delaySSTables) {
        // 맵에서 키 검색
        auto it = ss->rows.find(key);
        if (it != ss->rows.end()) {
            cout<<"(found in delaySStale:"<<ss->sstableId<<")";
            return it->second;  // 키를 찾았으면 값 반환
        }
    }

    //없어요
    return -1;
}

map<uint64_t, int> MockDisk::range(uint64_t start, uint64_t end) {

    // 로깅
    list<string> normalSSTableIds;
    list<string> delaySSTableIds;

    map<uint64_t, int> results;
    bool flag;
    for (auto ss : normalSSTables) {
        flag = false;
        for (auto it = ss->rows.lower_bound(start); it != ss->rows.end() && it->first <= end; ++it) {
            flag = true;
            results[it->first] = it->second;
        }
        if(flag) normalSSTableIds.push_back("("+to_string(ss->sstableId)+")");
    }
    for (auto ss : delaySSTables) {
        flag = false;
        for (auto it = ss->rows.lower_bound(start); it != ss->rows.end() && it->first <= end; ++it) {
            flag = true;
            results[it->first] = it->second;
        }
        if(flag) delaySSTableIds.push_back("("+to_string(ss->sstableId)+")");
    }
    // 로깅
    if(!normalSSTableIds.empty()) {
        cout << "found in normalSSTables ";
        for (auto id: normalSSTableIds) cout << id;
        cout <<"\n";
    }
    if(!delaySSTableIds.empty()) {
        cout<<"found in delaySSTables ";
        for(auto id : delaySSTableIds) cout << id;
        cout <<"\n";
    }

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
        // TODO : Flush count
        doFlush(memtable->mem.size());
    } else if (auto delayPtr = dynamic_cast<DelayMemtable*>(memtable)) {
        newSSTable->setType(D);
        delaySSTables.push_back(newSSTable);
        // TODO : Flush count
        doFlush(memtable->mem.size());
    }

    return true;
}

void MockDisk::printSSTableList() {

    cout<<"\n============NormalSSTable===========\n";
    for(auto table: normalSSTables){
        cout << "[ " << table->sstableId << " ]  key: " << table->startKey << " ~ " << table->lastKey << " | #cnt: " << table->rows.size() << "\n";
    }
    cout<<"\n============DelaySSTable===========\n";
    for(auto table: delaySSTables){
        cout << "[ " << table->sstableId << " ]  key: " << table->startKey << " ~ " << table->lastKey << " | #cnt: " << table->rows.size() << "\n";
    }

}
