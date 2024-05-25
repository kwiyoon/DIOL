#include "ActiveMemtableController.h"
#include "DBManager.h"

bool ActiveMemtableController::insert(uint64_t key, int value) {
    if(!isDelayData(key)) {
        return insertData(*activeNormalMemtable, key, value);
    }
    else {
        return insertData(*activeDelayMemtable, key, value);
    }
}

bool ActiveMemtableController::isDelayData(uint64_t key) {
    if(activeNormalMemtable->mem.empty()){
        return false;
    }else {
        return activeNormalMemtable->mem.begin()->first > key;
    }
}

bool ActiveMemtableController::insertData(IMemtable& memtable, uint64_t key, int value){
    DBManager& dbmanager = DBManager::getInstance();
    if (memtable.isFull()) {
        try {
            IMemtable* newMemtable = dbmanager.transformM0ToM1(&memtable);
            newMemtable->setStartKey(key);
            newMemtable->put(key, value);
            return true;
        } catch (exception &e) {
            cerr << e.what() << "\n";
        }
    }
    memtable.put(key, value);

    return true;
}

NormalMemtable* ActiveMemtableController::updateNormalMem(int id) {
    activeNormalMemtable = new NormalMemtable(id);
    return activeNormalMemtable;
}

DelayMemtable* ActiveMemtableController::updateDelayMem(int id) {
    activeDelayMemtable = new DelayMemtable(id);
    return activeDelayMemtable;
}