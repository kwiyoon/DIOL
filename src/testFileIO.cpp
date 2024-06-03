#include "test/FileIOTest.h"
#include <ctime>
#include <iostream>

void originMakeWorkload(list<Record>& initDataSet, list<Record>& initTxnSet, double readProportion, double insertProportion, double singleReadProportion, double rangeProportion){
    int initFileRecordCount = initDataSet.size();
    int singleReadCount = initFileRecordCount * (readProportion / insertProportion) * singleReadProportion;
    int rangeCount = initFileRecordCount * (readProportion / insertProportion) * rangeProportion;
    int randomReadKey;


    cout<<"single read 생성 진행"<<endl;
    clock_t start, finish;
    double duration;
    start = clock();
    for(int i=0; i< singleReadCount; i++){

        randomReadKey = rand() % initFileRecordCount + 1;
        Record record;
        record.key = randomReadKey;
        record.op = "READ";
        auto it = std::next(initTxnSet.begin(), randomReadKey);
        initTxnSet.insert(it, record);

        if (i != 0 && i % (singleReadCount / 100) == 0) {
            INT_LOG_PROGRESS(i, singleReadCount);
            finish = clock();
            duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout <<"ㄴ> "<< duration << "초" << endl;
            start = clock();
        }
    }

    cout<<"range 생성 진행"<<endl;

    for(int i=0; i<rangeCount; i++){
        int rangeStart = rand() % initFileRecordCount + 1;
        int rangeEnd = rand() % initFileRecordCount + 1;
        Record record;
        record.start_key = rangeStart;
        record.end_key = rangeEnd;
        record.op = "RANGE";
        if (i != 0 && i % (rangeCount / 100) == 0) {
            INT_LOG_PROGRESS(i, rangeCount);
        }
    }
}

void refactorMakeWorkload(list<Record>& initDataSet, list<Record>& initTxnSet, double readProportion, double insertProportion, double singleReadProportion, double rangeProportion){
    int initFileRecordCount = initDataSet.size();
    int singleReadCount = initFileRecordCount * (readProportion / insertProportion) * singleReadProportion;
    int rangeCount = initFileRecordCount * (readProportion / insertProportion) * rangeProportion;
    int randomReadKey;


    cout<<"single read 생성 진행"<<endl;
    clock_t start, finish;
    double duration;
    start = clock();
    for(int i=0; i< singleReadCount; i++){
        randomReadKey = rand() % initFileRecordCount + 1;

        Record record("READ", randomReadKey);
        std::list<Record>::iterator it;

        if (randomReadKey <= initTxnSet.size() / 2) {
            // 리스트의 앞쪽에서부터 접근
            it = std::next(initTxnSet.begin(), randomReadKey);
        } else {
            // 리스트의 뒤쪽에서부터 접근
            it = std::prev(initTxnSet.end(), initTxnSet.size() - randomReadKey);
        }
        initTxnSet.insert(it, record);

        if (i != 0 && i % (singleReadCount / 100) == 0) {
            INT_LOG_PROGRESS(i, singleReadCount);
            finish = clock();
            duration = (double)(finish - start) / CLOCKS_PER_SEC;
            cout <<"ㄴ> "<< duration << "초" << endl;
            start = clock();
        }
    }

    cout<<"range 생성 진행"<<endl;

    for(int i=0; i<rangeCount; i++){
        int rangeStart = rand() % initFileRecordCount + 1;
        int rangeEnd = rand() % initFileRecordCount + 1;
        Record record("RANGE", rangeStart, rangeEnd);
        std::list<Record>::iterator it;

        if (randomReadKey <= initTxnSet.size() / 2) {
            // 리스트의 앞쪽에서부터 접근
            it = std::next(initTxnSet.begin(), randomReadKey);
        } else {
            // 리스트의 뒤쪽에서부터 접근
            it = std::prev(initTxnSet.end(), initTxnSet.size() - randomReadKey);
        }
        initTxnSet.insert(it, record);
        if (i != 0 && i % (rangeCount / 100) == 0) {
            INT_LOG_PROGRESS(i, rangeCount);
        }
    }
}

int main(int argc, char* argv[]) {


    FileIOTest fo;

    std::string initDataName = argv[1]; //workload를 생성할 초기 dataset 파일 명
    std::string workloadDataName = argv[2]; //workload 파일 명
    double readProportion = stod(argv[3]); // 읽기 작업 비율
    double insertProportion = stod(argv[4]); // 삽입 작업 비율
    double singleReadProportion = stod(argv[5]); // 단일 read 작업에 대한 비율
    double rangeReadProportion = stod(argv[6]); // range 읽기 작업 비율

    /** read 시간 측정 */
    clock_t start, finish;
    double duration;
    start = clock();
    /** ============ */
    string initfilePath = "../src/test/dataset/" + initDataName;
    int halfLines = fo.extractHalfLinesFromFilename(initfilePath);


//    list<Record> initDataSet =  fo.readFile(initfilePath);
    list<Record> initDataSet =  fo.readFileFromStart(initfilePath, halfLines);
    list<Record> initTxnSetOrigin =  fo.readFileFromMiddle(initfilePath, halfLines);
    /** 종료 */
    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout <<"read DataSet : "<< duration << "초" << endl;

    list<Record> initTxnSetRefactor =  fo.readFileFromMiddle(initfilePath, halfLines);

    /** 기존 record 시간 측정 */
    start = clock();
    cout << "originMakeWorkload start";

    originMakeWorkload(initDataSet, initTxnSetOrigin, readProportion,insertProportion,singleReadProportion,rangeReadProportion);

    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout <<"originMakeWorkload : "<< duration << "초" << endl;



    /** 개선된 record 시간 측정 */
    start = clock();
    cout << "refactorMakeWorkload start";

    refactorMakeWorkload(initDataSet, initTxnSetRefactor, readProportion,insertProportion,singleReadProportion,rangeReadProportion);

    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout <<"refactorMakeWorkload : "<< duration << "초" << endl;


//    /** generate 시간 측정 */
//    start = clock();
//
////    DataFactory factory;
////    factory.generateWorkloadDatasetV2(initDataName, initDataSet, workloadDataName, readProportion, insertProportion, singleReadProportion, rangeReadProportion);
//    /** 종료 */
//    finish = clock();
//
//    duration = (double)(finish - start) / CLOCKS_PER_SEC;
//    cout <<"generate workload : "<< duration << "초" << endl;

    /** 기존 write 시간 측정 */
    start = clock();

    fo.writeToFile(workloadDataName+"origin", initTxnSetOrigin);

    /** 종료 */
    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout <<"origin writeToFile : "<< duration << "초" << endl;


    /** refactor write 시간 측정 */
    start = clock();

    fo.writeToFileV2(workloadDataName+"refactor", initTxnSetRefactor);

    /** 종료 */
    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout <<"refactor writeToFile : "<< duration << "초" << endl;

    return 0;

}