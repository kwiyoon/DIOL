#include "DelayDetector.h"

// delay 개수 추정
long DelayDetector::detect(IMemtable* memtable) {
    return (memtable->lastKey - memtable->startKey) - memtable->mem.size();
}