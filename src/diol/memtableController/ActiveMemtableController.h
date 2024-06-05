#ifndef ACTIVEMEMTABLECONTROLLER_H
#define ACTIVEMEMTABLECONTROLLER_H

#include "../memtable/IMemtable.h"
#include <vector>
#include <algorithm>

using namespace std;

class ActiveMemtableController {
public:
    static ActiveMemtableController& getInstance(){
        static ActiveMemtableController instance;
        return instance;
    }

    NormalMemtable* activeNormalMemtable;
    DelayMemtable* activeDelayMemtable;

    bool insert(uint64_t key, int value);
    bool isDelayData(uint64_t key);
    bool insertData(IMemtable& memtable, uint64_t key, int value);
    NormalMemtable* updateNormalMem(int id, uint64_t lastKey);
    DelayMemtable* updateDelayMem(int id);
private:
    ActiveMemtableController(){
        activeNormalMemtable = new NormalMemtable(1);
        activeDelayMemtable = new DelayMemtable(2);
    }
    ActiveMemtableController(const ActiveMemtableController&) = delete;
    void operator=(const ActiveMemtableController&) = delete;
};

#endif // ACTIVEMEMTABLECONTROLLER_H
