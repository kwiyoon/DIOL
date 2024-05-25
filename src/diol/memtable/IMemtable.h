#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <map>

enum State{
    M0,         // ACTIVE
    M1, M2      // IMM
};
//
//// TODO (new) : refactor
//enum Type{
//    N,  // Normal
//    D   // Delay
//};

// Abstract class for memtables
class IMemtable {
public:
    std::map<uint64_t, int> mem;
    State state;
    uint64_t startKey;
    uint64_t lastKey;
//    size_t memtableSize = 16 * 1024 * 1024;
    size_t memtableSize = 4 * 1024; // delay_size * 4 = normal_size
    int memtableId;

    int access = 0;     // IMM만 사용
    int ttl;       // IMM만 사용
//    static const int STEP1_TTL = INIT_TTL/2;
    static const int INIT_TTL = 10;

    virtual ~IMemtable() = default;
    void increaseAccessCount(int cnt);

    bool put(uint64_t key, int value);
    bool isFull();
    size_t getSize();

    bool initM1();
    bool setState(State state);
    bool setStartKey(uint64_t key);
    bool setLastKey(uint64_t key);
    void initTTL();
};

class DelayMemtable : public IMemtable {
public:
    DelayMemtable(int id);

    size_t memtableSize = memtableSize/16; // Normal/4
};

class NormalMemtable : public IMemtable {
public:
    NormalMemtable(int id);
    void setDelayCount(int cnt);
    size_t memtableSize = 4 * 1024;

    int delayCount = 0; // Normal IMM만 사용
};

#endif // MEMTABLE_H
