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
            dbmanager.transformM0ToM1(&memtable);
            activeNormalMemtable->memTableStatus = INSERTING;
            if(activeNormalMemtable->startKey>key){  //delay data
                insertData(*activeDelayMemtable, key, value);
            }else{ //normal data
                activeNormalMemtable->setStartKey(key);
                activeNormalMemtable->put(key, value);
                activeNormalMemtable->memTableStatus = WORKING;
            }
            return true;
        } catch (exception &e) {
            cerr << e.what() << "\n";
        }
    }
    memtable.memTableStatus = INSERTING;
    memtable.put(key, value);
    memtable.memTableStatus = WORKING;

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