#ifndef DELAYMEMTABLE_H
#define DELAYMEMTABLE_H

#include "IMemtable.h"

class DelayMemtable : public IMemtable {
public:
    DelayMemtable(int id);

    size_t memtableSize = memtableSize/16; // Normal/4
};

#endif //DELAYMEMTABLE_H
