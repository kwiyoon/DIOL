//
// Created by 이해나 on 5/25/24.
//
#include "test/DataFactory.h"
#include "test/workload/Workload.h"
#include "test/CompactionTest.h"

int main(int argc, char* argv[]) {
    Workload workloadA(DBManager::getInstance(), MockDisk::getInstance());
    string initDataNum = argv[1]; //ex) "10000";
    string initFileName = argv[2]; //ex) "dataB_c20000000_d0.2";
    string workloadFileName = argv[3]; //ex) "workloadA_r0.3_i0.7_V2";

    string initFilePath = "../src/test/dataset/" + initFileName+".txt";
    string workloadFilePath = "../src/test/dataset/workload/" + workloadFileName+".txt";
    int halfLines = workloadA.extractHalfLinesFromFilename(initFilePath);


    /**init dataset의 halfline만큼 읽어서 워크로드 진행 전 선행시킬 INSERT작업 데이터셋 저장*/
    std::list<Record> initWorkload = workloadA.readFileFromStart(initFilePath, halfLines);
    /**실질 INSERT, READ, RANGE 워크로드 데이터셋 파일 읽어서 저장*/
    std::list<Record> mixedWorkload = workloadA.readFileWhole(workloadFilePath);



    //initWorkload을 이용해서 INSERT 작업 진행
    workloadA.executeWorkload(initWorkload, false);
    //mixedWorkload을 이용해서 INSERT, READ, RANGE 작업 진행
    workloadA.executeWorkload(mixedWorkload, true);

    MockDisk& disk = MockDisk::getInstance();

    cout<<"\ndisk read 횟수 : "<< disk.readCount<<"\n";

    workloadA.printDelayData();

    workloadA.deleteAllSSTable();

    workloadA.makeSSTable();

    CompactionTest compacton;
    compacton.runCompaction();




}