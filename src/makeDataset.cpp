//
// Created by 이해나 on 5/25/24.
//
#include "test/DataFactory.h"

/**
 * init
 * */

int main(int argc, char* argv[]) {
    DataFactory factory;

    string dataSetName = argv[1]; //ex) "dataA";
    int initDataCount = atoi(argv[2]); // 데이터셋 크기
    // int numSegments = atoi(argv[2]); // out of ordner 세그먼트 개수
    double outOfOrderRatio = stod(argv[3]); // out of order 비율



    // cout<<initDataCount<<"  "<<numSegments<<"  "<<outOfOrderRatio<<"\n";

    factory.generateO3Dataset(dataSetName, initDataCount, outOfOrderRatio);

}