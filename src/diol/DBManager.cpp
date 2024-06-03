#include "DBManager.h"


bool DBManager::insert(uint64_t key, int value){
    return activeMemtableController.insert(key, value);
}
int DBManager::readData(uint64_t key){
    // 만약 해당 키가 액티브
    if(activeMemtableController.activeNormalMemtable->startKey < key)
        return NULL;
    return immMemtableController.read(key);
}
map<uint64_t , int> DBManager::range(uint64_t start, uint64_t end){
    return immMemtableController.range(start, end);
}
//bool DBManager::isFull(IMemtable& memtable){
//
//}

int DBManager::getIdAndIncrement(){
    return ++currentId;
}

IMemtable* DBManager::transformM0ToM1(IMemtable* memtable) {
    // TODO (논의) : 사실 INSERTING 전에 lock 걸고 작업후 해제하기 때문에.. 필요없을 수도 있음.
    //    while(memtable->memTableStatus==INSERTING);
//    {
    std::unique_lock<std::mutex> lock(memtable->mutex);

    ActiveMemtableController& activeController = ActiveMemtableController::getInstance();
    ImmutableMemtableController& immutableController = ImmutableMemtableController::getInstance();

    uint64_t minKey = numeric_limits<uint64_t>::max();
    uint64_t maxKey = numeric_limits<uint64_t>::min();

    auto updateKeys = [&](IMemtable* memtablePtr) {
        for (const auto& entry : memtablePtr->mem) {
            if (entry.first < minKey) minKey = entry.first;
            if (entry.first > maxKey) maxKey = entry.first;
        }
        memtablePtr->initM1();
        if (minKey != numeric_limits<uint64_t>::max()) {
            memtablePtr->setStartKey(minKey);
        }
        if (maxKey != numeric_limits<uint64_t>::min()) {
            memtablePtr->setLastKey(maxKey);
        }
        immutableController.putMemtableToM1List(memtablePtr);
        lock.unlock();
    };

    if (auto normalPtr = dynamic_cast<NormalMemtable*>(memtable)){
        immutableController.decreaseTTL();
        updateKeys(normalPtr);
        normalPtr->setDelayCount(DelayDetector::detect(normalPtr));
        return activeController.updateNormalMem(getIdAndIncrement(), normalPtr->lastKey);
    } else if (auto delayPtr = dynamic_cast<DelayMemtable*>(memtable)){
        updateKeys(delayPtr);
        return activeController.updateDelayMem(getIdAndIncrement());
    }else
        throw logic_error("DBManager::transformM0ToM1 주소비교.. 뭔가 문제가 있는 듯 하오.");
//    }
}
