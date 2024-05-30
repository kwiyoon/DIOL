#ifndef IMMUTABLEMEMTABLECONTROLLER_H
#define IMMUTABLEMEMTABLECONTROLLER_H

#include "../memtable/IMemtable.h"
#include "../CompactProcessor.h"
#include "../MockDisk.h"
#include "../CompactionController.h"
#include <iostream>
#include <queue>


#define LOG_STR(str) \
//    cout<<str<<endl;

#define LOG_ID(id) \
//    cout<<"(found in id:"<<id<<")\n";

using namespace std;

//static const int LIMIT_SIZE_M1 = 3;
//static const int LIMIT_SIZE_M2 = 3;

static const int LIMIT_SIZE_NORMAL_M1 = 5;
static const int LIMIT_SIZE_DELAY_M1 = 8;
static const int LIMIT_SIZE_NORMAL_M2 = 5;
static const int LIMIT_SIZE_DELAY_M2 = 7;

class FlushController;

class ImmutableMemtableController {
public:
    static ImmutableMemtableController& getInstance(){
        static ImmutableMemtableController instance;
        return instance;
    }

    vector<IMemtable*> normalImmMemtableList_M1; // limit LIMIT_SIZE_NORMAL_M1
    vector<IMemtable*> delayImmMemtableList_M1; // limit LIMIT_SIZE_DELAY_M1
    vector<IMemtable*> normalImmMemtableList_M2; // limit LIMIT_SIZE_NORMAL_M2
    vector<IMemtable*> delayImmMemtableList_M2; // limit LIMIT_SIZE_DELAY_M2
    queue<IMemtable*> compactionQueue; // if M1 full -> insert compaction Queue
    queue<IMemtable*> flushQueue;
    CompactProcessor* compactProcessor;
    FlushController* flushController = nullptr;
    MockDisk& disk;

    int read(uint64_t key);
    map<uint64_t, int> range(uint64_t start, uint64_t end);
    void putMemtableToM1List(IMemtable*);
    void decreaseTTL();
    void erase(vector<IMemtable*>& v, IMemtable* memtable);
    void transformM1toM2(IMemtable*);
    void setFlushController(FlushController* fc){
        this->flushController = fc;
    }
private:
    ImmutableMemtableController() : disk(MockDisk::getInstance()) {
        compactProcessor = new CompactProcessor();
    }

    ImmutableMemtableController(const ImmutableMemtableController&) = delete;
    void operator=(const ImmutableMemtableController&) = delete;

    void compaction();
    void convertOldDelayToM2();
    int diskRead(uint64_t key);
    map<uint64_t, int> diskRange(uint64_t start, uint64_t end);
    map<uint64_t, int> rangeInVector(uint64_t start, uint64_t end, vector<IMemtable*>& v, Type t);
    int readInVector(uint64_t key, vector<IMemtable*>& v);
};

#endif // IMMUTABLEMEMTABLECONTROLLER_H
