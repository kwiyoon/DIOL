#ifndef DELAYDETECTOR_H
#define DELAYDETECTOR_H

#include "memtable/IMemtable.h"

using namespace std;

class DelayDetector {
public:
    static long detect(IMemtable* memtable);
};


#endif // DELAYDETECTOR_H
