//
// Created by 이해나 on 5/25/24.
//
#include "test/DataFactory.h"
#include "test/workload/Workload.h"

int main(int argc, char* argv[]) {

    Workload workloadA(DBManager::getInstance(), MockDisk::getInstance());
    DataFactory factory;

    std::string initDataFileName = argv[1]; //workload를 생성할 초기 dataset 파일 명 ex) "dataA_c10000_d0.2.txt"
    std::string workloadDataName = argv[2]; //workload 데이터 이름 ex) workloadA
    double readProportion = stod(argv[3]); // 읽기 작업 비율
    double insertProportion = stod(argv[4]); // 삽입 작업 비율
    double singleReadProportion = stod(argv[5]); // 단일 read 작업에 대한 비율
    double rangeReadProportion = stod(argv[6]); // range 읽기 작업 비율

    string initfilePath = "../src/test/dataset/" + initDataFileName;
    vector<Record> initDataSet = workloadA.readFile(initfilePath);

    factory.generateWorkloadDataset(initDataSet,workloadDataName, readProportion, insertProportion, singleReadProportion, rangeReadProportion);
}