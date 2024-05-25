#include <iostream>
#include "diol/DBManager.h"
#include "test/DataFactory.h"
//#include "test/CompactionTest.h"

using namespace std;

int main() {
    DBManager& tree = DBManager::getInstance();
    for(int i=1;i<1000;i++){
        tree.insert(i,i*2);
    }

    for(int i=1;i<1000;i++){
        tree.readData(i);
    }


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
