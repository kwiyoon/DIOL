#include "ImmutableMemtableController.h"
#include "FlushController.h"

// 로깅 ====
list<string> ids;
bool flag;
// ======

map<uint64_t, int> ImmutableMemtableController::rangeInVector(uint64_t start, uint64_t end, vector<IMemtable*>& v){
    map<uint64_t, int> segment;

    for (auto imm : normalImmMemtableList_M1) {
        imm->memTableStatus = READING;
        flag = false;
        for (auto it = imm->mem.lower_bound(start); it != imm->mem.end() && it->first <= end; ++it) {
            segment[it->first] = it->second;
            flag = true;
        }
        if(flag) ids.push_back("("+to_string(imm->memtableId)+")");
        imm->memTableStatus = IMMUTABLE;
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

    /*
    if(!ids.empty()){
        cout << "found in immMemtables ";
        for (auto id: ids) cout << id;
        cout <<"\n";
    }
     */

    //병합
    results.insert(diskData.begin(), diskData.end());

    return results;
}

void ImmutableMemtableController::putMemtableToM1List(IMemtable* memtable) {
    if(LIMIT_SIZE_NORMAL_M1*0.8 <= normalImmMemtableList_M1.size()){
        compaction();
    }
    if(LIMIT_SIZE_DELAY_M1*0.8 <= delayImmMemtableList_M1.size()){
        convertOldDelayToM2();
    }
    if (auto normalPtr = dynamic_cast<NormalMemtable*>(memtable)){
        normalImmMemtableList_M1.push_back(normalPtr);
    }
    else if (auto delayPtr = dynamic_cast<DelayMemtable*>(memtable)){
        delayImmMemtableList_M1.push_back(delayPtr);
    }
}

int ImmutableMemtableController::readInVector(uint64_t key, vector<IMemtable*>& v){
    for (auto imm : v) {
        imm->memTableStatus = READING;
        if(imm->startKey > key || imm->lastKey < key) continue;
        // 맵에서 키 검색
        auto it = imm->mem.find(key);
        if (it != imm->mem.end()) {
            LOG_ID(imm->memtableId);
            imm->increaseAccessCount(1);
            return it->second;  // 키를 찾았으면 값 반환
        }
        imm->memTableStatus = IMMUTABLE;
    }
    return NULL;
}

int ImmutableMemtableController::read(uint64_t key) {
    int value = readInVector(key, normalImmMemtableList_M1);
    if(value == NULL) {
        value = readInVector(key, normalImmMemtableList_M2);
    }
    if(value == NULL) {
        value = readInVector(key, delayImmMemtableList_M1);
    }
    if(value == NULL) {
        value = readInVector(key, delayImmMemtableList_M2);
    }

    if(value != NULL) return value;
    return diskRead(key); //빈함수
}

int ImmutableMemtableController::diskRead(uint64_t key){
//    cout<<"reading Disk data~";
    disk.readCount++;

    return disk.read(key);
}

map<uint64_t, int> ImmutableMemtableController::diskRange(uint64_t start, uint64_t end){
//    cout<<"ranging Disk datas~ ";
    map<uint64_t, int> diskData = disk.range( start, end);
    disk.readCount += diskData.size();

    return diskData;
}

void ImmutableMemtableController::convertOldDelayToM2(){
    LOG_STR("ImmutableMemtableController::convertOldDelayToM2()");
    int min = std::numeric_limits<int>::max();
    IMemtable* target = NULL;
    for (const auto memtable : delayImmMemtableList_M1) {
        if (memtable->ttl <= min) {
            min = memtable->ttl;
            target = memtable;
        }
    }
    transformM1toM2(target);
}

void ImmutableMemtableController::compaction() {
    if(compactionQueue.empty()){
        CompactionController compactionController;
//        compactionController.checkTimeOut();
        compactionController.start();
        compactionController.waitForCompletion();
        compactionController.stop();
        LOG_STR("start까지 ㄱㅊ");
    }

    IMemtable* normalMemtable = compactionQueue.front();
    LOG_STR("normalMemtable id: " + to_string(normalMemtable->memtableId));


    if(!delayImmMemtableList_M1.empty()){
        IMemtable* delaymemtable = compactProcessor->compaction(normalMemtable, delayImmMemtableList_M1);

        transformM1toM2(delaymemtable);
    }
    transformM1toM2(normalMemtable);
    compactionQueue.pop();
    LOG_STR("ImmutableMemtableController::compaction 끝!");
}

void ImmutableMemtableController::erase(vector<IMemtable*>& v, IMemtable* memtable){
    for(auto it = v.begin(); it != v.end();){
        auto element = *it;
        if(element == memtable){
            it = v.erase(it);
        } else it++;
    }
    LOG_STR("erase 잘돼요");
}
//
//void ImmutableMemtableController::setFlushController(FlushController* controller) {
//    this->flushController = controller;
//}


void ImmutableMemtableController::transformM1toM2(IMemtable* memtable) {
    if(LIMIT_SIZE_NORMAL_M2*0.8 <= normalImmMemtableList_M2.size()){
        cout<<"\n==========full!!============"<<endl;
        flushController->start(N);
        flushController->waitForCompletion();
        flushController->stop();
    }
    if(LIMIT_SIZE_DELAY_M2*0.8 <= delayImmMemtableList_M2.size()){
        cout<<"\n==========full!!============"<<endl;
        flushController->start(D);
        flushController->waitForCompletion();
        flushController->stop();
    }

    memtable->initM2();
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