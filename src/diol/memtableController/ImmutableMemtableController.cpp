#include "ImmutableMemtableController.h"

// 로깅 ====
list<string> ids;
bool flag;
// ======


map<uint64_t, int> ImmutableMemtableController::rangeInVector(uint64_t start, uint64_t end, vector<IMemtable*>& v){
    map<uint64_t, int> segment;

    for (auto imm : normalImmMemtableList_M1) {
        flag = false;
        for (auto it = imm->mem.lower_bound(start); it != imm->mem.end() && it->first <= end; ++it) {
            segment[it->first] = it->second;
            flag = true;
        }
        if(flag) ids.push_back("("+to_string(imm->memtableId)+")");
    }

    return segment;
}

map<uint64_t, int> ImmutableMemtableController::range(uint64_t start, uint64_t end) {
    ids.clear();
    flag = false;

    map<uint64_t, int> results = rangeInVector(start, end, normalImmMemtableList_M1);
    map<uint64_t, int> segment = rangeInVector(start, end, normalImmMemtableList_M2);
    results.insert(segment.begin(), segment.end());

    segment = rangeInVector(start, end, delayImmMemtableList_M1);
    results.insert(segment.begin(), segment.end());

    segment = rangeInVector(start, end, delayImmMemtableList_M2);
    results.insert(segment.begin(), segment.end());

    // 만약 start 범위가 disk일 가능성이 있을때
    map<uint64_t, int> diskData;

    if(results.empty() || start < results.begin()->first || end > results.rbegin()->first){
        diskData=diskRange(start, end);
    }

    if(!ids.empty()){
        cout << "found in immMemtables ";
        for (auto id: ids) cout << id;
        cout <<"\n";
    }

    //병합
    results.insert(diskData.begin(), diskData.end());

    return results;
}

int ImmutableMemtableController::getM1ListsSize(){
    return normalImmMemtableList_M1.size() + delayImmMemtableList_M1.size();
}

int ImmutableMemtableController::getM2ListsSize(){
    return normalImmMemtableList_M2.size() + delayImmMemtableList_M2.size();
}

void ImmutableMemtableController::putMemtableToQueue(IMemtable* memtable) {
    if(LIMIT_SIZE_M1 == getM1ListsSize()){
        compaction();
    }
    if (auto normalPtr = dynamic_cast<NormalMemtable*>(memtable)){
        normalImmMemtableList_M1.push_back(normalPtr);
    }
    else if (auto delayPtr = dynamic_cast<DelayMemtable*>(memtable)){
        delayImmMemtableList_M1.push_back(delayPtr);
    }
}

int ImmutableMemtableController::readInVector(uint64_t key, vector<IMemtable*>& v){
    for (auto imm : delayImmMemtableList_M1) {
        // 맵에서 키 검색
        auto it = imm->mem.find(key);
        if (it != imm->mem.end()) {
            cout<<"(found in id:"<<imm->memtableId<<")";
            imm->increaseAccessCount(1);
            return it->second;  // 키를 찾았으면 값 반환
        }
    }
}

int ImmutableMemtableController::read(uint64_t key) {
    readInVector(key, normalImmMemtableList_M1);
    readInVector(key, normalImmMemtableList_M2);
    readInVector(key, delayImmMemtableList_M1);
    readInVector(key, delayImmMemtableList_M2);

    return diskRead(key); //빈함수
}

int ImmutableMemtableController::diskRead(uint64_t key){
    cout<<"reading Disk data~";
    disk->readCount++;

    return disk->read(key);
}

map<uint64_t, int> ImmutableMemtableController::diskRange(uint64_t start, uint64_t end){
    cout<<"ranging Disk datas~ ";
    map<uint64_t, int> diskData = disk->range( start, end);
    disk->readCount += diskData.size();

    return diskData;
}


void ImmutableMemtableController::compaction() {
    // TODO : compactProcessor.compaction();
    IMemtable* normalMemtable = compactionQueue.front();
    IMemtable* delaymemtable = compactProcessor->compaction(normalMemtable, delayImmMemtableList_M1);
    compactionQueue.pop();
    transformM1toM2(normalMemtable);
    transformM1toM2(delaymemtable);
}

void ImmutableMemtableController::erase(vector<IMemtable*>& v, IMemtable* memtable){
    for(auto it = v.begin(); it != v.end();){
        auto element = *it;
        if(element == memtable){
            it = v.erase(it);
        } else it++;
    }
}

void ImmutableMemtableController::transformM1toM2(IMemtable* memtable) {
    memtable->setState(M2);
    memtable->initTTL();
    if (NormalMemtable* normalPtr = dynamic_cast<NormalMemtable*>(memtable)){
        erase(normalImmMemtableList_M1, normalPtr);
        normalImmMemtableList_M2.push_back(normalPtr);
    } else if (DelayMemtable* delayPtr = dynamic_cast<DelayMemtable*>(memtable)){
        erase(delayImmMemtableList_M1, delayPtr);
        delayImmMemtableList_M2.push_back(delayPtr);
    }
    else
        throw logic_error("ImmutableMemtableController::transformM1toM2 주소비교.. 뭔가 문제가 있는 듯 하오.");
}

// 모든 M1 memtable에 ttl--
void ImmutableMemtableController::decreaseTTL() {
    for (auto imm : normalImmMemtableList_M1) {
        imm->ttl--;
    }
}