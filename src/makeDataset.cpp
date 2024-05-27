//
// Created by 이해나 on 5/25/24.
//
#include "test/DataFactory.h"

/**
 * init
 * */

int main(int argc, char* argv[]) {
    DataFactory factory;

    int initDataCount = atoi(argv[1]); // 데이터셋 크기
    // int numSegments = atoi(argv[2]); // out of ordner 세그먼트 개수
    double outOfOrderRatio = stod(argv[2]); // out of order 비율
    string dataSetName = argv[3]; //ex) "dataA";


    // cout<<initDataCount<<"  "<<numSegments<<"  "<<outOfOrderRatio<<"\n";

    factory.generateDelayedDataset(dataSetName, initDataCount, outOfOrderRatio);

}