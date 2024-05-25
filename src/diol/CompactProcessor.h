#ifndef COMPACTPROCESSOR_H
#define COMPACTPROCESSOR_H

#include "memtable/IMemtable.h"
#include <iostream>

using namespace std;

// TODO : CompactProcessor 구현
// Compaction Queue의 맨 앞에 있는 memtable의 compaction을 수행합니다.
// 이 때, 시간이 가장 많이 겹치는 Delay Memtable을 찾아 부분 compaction을 진행합니다.
// 1. Immutable Memtable Controller에게로부터 compaction 요청이 들어옵니다.
// 2. Compaction Queue에서 맨 앞에 있는 Memtable과 시간이 가장 많이 겹치는 Delay Memtable을 찾습니다.
// 3. 찾은 두 Memtable을 compaction 한 후 Immutable Memtable Controller 에게 compaction한 DelayMemtable의 주소를 반환합니다.
class CompactProcessor {
public:
    DelayMemtable* compaction(IMemtable* memtable, vector<IMemtable*>& delayMemtables);
};


#endif // COMPACTPROCESSOR_H
