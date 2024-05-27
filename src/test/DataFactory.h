
#ifndef dataFactory_H
#define dataFactory_H

#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cstring>



#include "../diol/DBManager.h"

#define VECTOR_LOG_PROGRESS(iteration, datasetSize) \
    if ((iteration * 100 / datasetSize.size()) % 10 == 0) { \
        cout << (iteration * 100 / datasetSize.size()) << "% \n"; \
    }

#define INT_LOG_PROGRESS(iteration, Count) \
    if ((iteration * 100 / Count) % 10 == 0) { \
        cout << (iteration * 100 / Count) << "% \n"; \
    }

using namespace std;
struct Record {
    string op;
    uint64_t key;
    uint64_t start_key;
    uint64_t end_key;
};
class DataFactory {

public:
    void generateNormalDataset(int n);
    void NormalTest();
    void generateDelayedDataset(string& dataSetName, int dataNum, double outOfOrderRatio);
    void delayedTest();

    void writeToInitFile(string filePath, vector<uint64_t>& dataset);

    //N바이트 쓸때 시간 측정

    void writeToFile(size_t bytes);
    void readFromFile(size_t bytes);
    void generateWorkloadDataset(string initDataName,vector<Record>& initDataSet, string& workloadDataName, double readProportion, double insertProportion, double singleReadProportion, double rangeProportion);
    void writeToWorkloadFile(string filePath, vector<Record>& dataset);
private:
    DBManager& tree = DBManager::getInstance();
    int outOfOrderCount;    //o3 data 몇개인지 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    uint64_t randomIndex;  //o3 data가 삽입되는 인덱스 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    string filename="dump.txt";
    vector<int> sizes; // 각 segment의 크기를 저장할 벡터
};

#endif //dataFactory_H