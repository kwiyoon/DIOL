#ifndef IMMUTABLEMEMTABLECONTROLLER_H
#define IMMUTABLEMEMTABLECONTROLLER_H

#include "../memtable/IMemtable.h"
#include "../memtable/NormalMemtable.h"
#include "../memtable/DelayMemtable.h"
#include "../CompactProcessor.h"
#include "../MockDisk.h"
#include <iostream>
#include <queue>

using namespace std;

static const int LIMIT_SIZE_M1 = 3;
static const int LIMIT_SIZE_M2 = 3;

class ImmutableMemtableController {
public:
    static ImmutableMemtableController& getInstance(){
        static ImmutableMemtableController instance;
        return instance;
    }

    static vector<IMemtable*> normalImmMemtableList_M1;
    static vector<IMemtable*> delayImmMemtableList_M1;
    static vector<IMemtable*> normalImmMemtableList_M2;
    static vector<IMemtable*> delayImmMemtableList_M2;
    queue<IMemtable*> compactionQueue;
    queue<IMemtable*> flushQueue;
    CompactProcessor* compactProcessor;

    int normalMemtableNum_M1 = 2;
    int delayMemtableNum_M1 = normalMemtableNum_M1 * 2;
    int read(uint64_t key);
    
    MockDisk* disk;
    map<uint64_t, int> range(uint64_t start, uint64_t end);
    bool isNormalMemsFull();
    bool isDelayMemsFull();
    void putMemtableToQueue(IMemtable*);
    void decreaseTTL();
    static void erase(vector<IMemtable*>& v, IMemtable* memtable);
    static void transformM1toM2(IMemtable*);
private:
    ImmutableMemtableController(){
        compactProcessor = new CompactProcessor();
        disk = new MockDisk();
    }

    ImmutableMemtableController(const ImmutableMemtableController&) = delete;
    void operator=(const ImmutableMemtableController&) = delete;

    void compaction();
    int getM1ListsSize();
    int getM2ListsSize();
    int diskRead(uint64_t key);
    map<uint64_t, int> diskRange(uint64_t start, uint64_t end);
    map<uint64_t, int> rangeInVector(uint64_t start, uint64_t end, vector<IMemtable*>& v);
    int readInVector(uint64_t key, vector<IMemtable*>& v);
};

#endif // IMMUTABLEMEMTABLECONTROLLER_H
