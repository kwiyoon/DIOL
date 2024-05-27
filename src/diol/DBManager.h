#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "memtable/IMemtable.h"
#include "DelayDetector.h"
#include "memtableController/ActiveMemtableController.h"
#include "memtableController/ImmutableMemtableController.h"
#include "CompactionController.h"
#include "FlushController.h"

class FlushController;

class DBManager {
public:
    static DBManager& getInstance(){
        static DBManager instance;
        return instance;
    }
//    ~DBManager(){
//        compactionController->stop();
//        flushController->stop();
//    }
    int currentId = 0;
    int getIdAndIncrement();

    bool insert(uint64_t key, int value);
    int readData(uint64_t key);
    map<uint64_t, int> range(uint64_t start, uint64_t end);
//    bool isFull(IMemtable& memtable);
    IMemtable* transformM0ToM1(IMemtable* memtable); // normal? delay?
private:
    DBManager(): activeMemtableController(ActiveMemtableController::getInstance()),
                 immMemtableController(ImmutableMemtableController::getInstance()){
        currentId = 0;
        getIdAndIncrement();
//        compactionController->start();
//        flushController->start();
        immMemtableController.setFlushController(new FlushController(immMemtableController));
    }
    DBManager(const DBManager&) = delete;
    void operator=(const DBManager&) = delete;

//    CompactionController* compactionController = new CompactionController();
//    FlushController* flushController = new FlushController();

    ActiveMemtableController& activeMemtableController;
    ImmutableMemtableController& immMemtableController;
};


#endif // DBMANAGER_H