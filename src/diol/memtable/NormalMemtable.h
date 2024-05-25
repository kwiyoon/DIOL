#ifndef NORMALMEMTABLE_H
#define NORMALMEMTABLE_H

#include "IMemtable.h"

class NormalMemtable : public IMemtable {
public:
    NormalMemtable(int id);
    void setDelayCount(int cnt);
    size_t memtableSize = 4 * 1024;

    int delayCount = 0; // Normal IMM만 사용
};

#endif //NORMALMEMTABLE_H
