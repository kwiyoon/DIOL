#include <iostream>
#include "diol/DBManager.h"
#include "test/DataFactory.h"
//#include "test/CompactionTest.h"

#include "test/VectorTest.cpp"
using namespace std;

int main() {

    // TODO : DBManager thread로 바꿔서.. 하여튼 immList 꽉차면 compaction, flush 등등 되도록 해줘야 해!!
    //  : 지금은 너무 빨라서 FlushController랑 CompactionController가 껴들 틈이 없3..

//    VectorTest v;
//    v.test();

    DBManager& tree = DBManager::getInstance();
    int num = 1000;
    for(int i=1;i<num;i++){
        tree.insert(i,i*2);
    }

    for(int i=num+20;i<num*3;i++){
        tree.insert(i,i*2);
    }

    for(int i=1;i<num*3;i++){
        int value = tree.readData(i);
        if(value == NULL)
            cout<<i<<",NULL"<<"\n";
        else
            cout<<i<<","<<tree.readData(i)<<"\n";
    }

    return 0;

//    tree.insert(1,10);
//    cout<<tree.readData(1);

//    DataFactory factory;
//    int n = 1000; // 데이터셋 크기
//    int numSegments = 10; // out of order 세그먼트 개수
//    double outOfOrderRatio = 0.3; // out of order 비율
//
//    factory.deleteAllSSTable();
//    factory.generateDelayedDataset(n,outOfOrderRatio,numSegments);
//    factory.printDelayData();
//
//    CompactionTest compaction;
//    compaction.runCompaction();

    return 0;
}
