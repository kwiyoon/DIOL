
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
#include <set>



#include "../diol/DBManager.h"

#define VECTOR_LOG_PROGRESS(iteration, datasetSize) \
    if ((iteration * 100 / datasetSize.size()) % 5 == 0) { \
        cout << (iteration * 100 / datasetSize.size()) << "% \n"; \
    }

#define INT_LOG_PROGRESS(iteration, Count) \
     if ((iteration * 100 / Count) % 10 == 0) { \
        cout << (iteration * 100 / Count) << "% \n"; \
    }

// FILE_LOG_PROGRESS 함수 선언
#define FILE_LOG_PROGRESS(iteration, total) \
    do { \
        double progress = static_cast<double>(iteration) / total * 100; \
        std::cout << progress << "%" << std::endl; \
    } while (0)

using namespace std;
struct Record {
    string op;
    uint64_t key;
    uint64_t start_key;
    uint64_t end_key;
};


class DataFactory {

public:

    void writeToInitFile(string filePath,  set<int>& dataSet, vector<std::vector<int>> segmentRandomKeys, int lineToWrite);
    void generateReadRangeDataset(double readProportion, double insertProportion, double singleReadProportion, double rangeProportion, list<Record>& initDataSet);
    void transferLinesToWorkloadFile(const std::string &initFilePath, string& workloadDataName, int linesToRead, double readProportion,double insertProportion,double singleReadProportion);
    void generateDelaySegments(std::vector<std::vector<int>>& outOfOrderKeysPerSegment, int dataNum, int numOfSegments, int segmentDataNum);
    void generateO3Dataset(string& dataSetName, int dataNum, double outOfOrderRatio);
    std::map<int, int> generateSingleDelayDataset(set<int>& dataSet, vector<int> copyDataSet, int dataNum);
    void setSingleDelayOffset(int key, set<int>& dataSet);
    void setSegmentDelayOffset(const vector<int>& segment, size_t dataSetSize);

    //N바이트 쓸때 시간 측정
    void writeToFile(size_t bytes);
    void readFromFile(size_t bytes);

private:
    int outOfOrderCount;   //o3 data 몇개인지 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    uint64_t randomIndex;  //o3 data가 삽입되는 인덱스 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    string filename="dump.txt";
    vector<int> sizes; // 각 segment의 크기를 저장할 벡터
    int randomReadKey; // 랜덤 읽기를 위한 키
    unordered_map<int, Record> singleReadSet;
    unordered_map<int, Record> rangeSet;
    vector<int> segmentDelayOffsets;
    map<int, int>  singleDelayKeys;


    unordered_set<int> randomIndexMap;
};

#endif //dataFactory_H
