#include "../core/DBManager.h"
#include "DataFactory.h"
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

//o3 데이터 없이 데이터셋 생성
void DataFactory:: generateNormalDataset(int n){
    tree = new DBManager();
    for (uint64_t i = 1; i <= n; ++i) {
        auto data = make_pair(i, static_cast<int>(i * 2));
        tree->insert(data.first, data.second);
    }
}

void DataFactory:: NormalTest(){
    tree->printActiveMemtable(true);
    tree->printImmMemtable();
    tree->Disk->printSSTableList();

    //parameter case별로 테스트 결과 달라짐
//    cout<<"\n\n\n\n========read/range test=========\n";
//
//    cout<<"[Read] key : "<<2009<<" value : "<<tree->readData(2009)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<2010<<" value : "<<tree->readData(2010)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<2011<<" value : "<<tree->readData(2011)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<1000<<" value : "<<tree->readData(1000)<<"\n"; // normalSSTable read
//    cout<<"[Read] key : "<<1200<<" value : "<<tree->readData(1200)<<"\n"; // normalSSTable read
//    // Active에만 있는 data를 읽으려 할 때 불가! : -1
//    cout<<"[Read] key : "<<2390<<" value : "<<tree->readData(2390)<<"\n";
//
//    // 하나의 normal ImmMemtable range
//    cout<<"\n[range] 하나의 normal ImmMemtable range : 2020 ~ 2030\n";
//    map<uint64_t, int> result = tree->range(2020,2030);
//    for(auto data: result){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//    // 여러개의 normal ImmMemtable range
//    cout<<"\n[range] 여러개의 normal ImmMemtable range : 2005 ~ 2015\n";
//    map<uint64_t, int> result1 = tree->range(2005,2015);
//    for(auto data: result1){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // 한개의 SStalble range
//    cout<<"\n[range] 한개의 SStalble range : 700 ~ 710\n";
//    map<uint64_t, int> result2 = tree->range(700,710);
//    for(auto data: result2){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // 여러개의 SStalble range
//    cout<<"\n[range] 여러개의 SStalble range : 330 ~ 340\n";
//    map<uint64_t, int> result3 = tree->range(330,340);
//    for(auto data: result3){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // ImmTable과 SStable에 섞여있을때
//    cout<<"\n[range] ImmTable과 SStable에 섞여있을때 : 1670 ~ 1680\n";
//    map<uint64_t, int> result4 = tree->range(1670,1680);
//    for(auto data: result4){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
    delete tree;
}

// o3데이터 포함 데이터셋 생성 함수
void DataFactory:: generateDelayedDataset(int n, double outOfOrderRatio, int numSegments) {
    vector<pair<uint64_t, int>> dataset;
    tree = new DBManager();

    // out of order 데이터 개수 계산
     outOfOrderCount = static_cast<int>(n * outOfOrderRatio);


    //out of order 데이터 셋 생성
    vector<vector<uint64_t>> outOfOrderKeysPerSegment(numSegments);
    for (int segment = 0; segment < numSegments; ++segment) {
        outOfOrderCount = static_cast<int>(n * outOfOrderRatio / numSegments);
        for (int i = 1; i <= outOfOrderCount; ++i) {
            outOfOrderKeysPerSegment[segment].push_back(i + segment * (n / numSegments));
        }
    }

    // 노멀 데이터셋 생성
    cout << "Dataset:" << endl;
    for (int i = 1; i <= n; ++i) {
        dataset.push_back(make_pair(i, i * 2));
    }

    //out of order 데이터 구간 출력(이 구간을 확인해야 out of order 데이터 추가할 위치(index)를 결정할 수 있음)
    cout << "Out of order Dataset:" << endl;
    for (int segment = 0; segment < numSegments; ++segment) {
        cout << "=======Segment " << segment + 1 << "구간=======" << endl;

        const auto& outOfOrderKeys = outOfOrderKeysPerSegment[segment];

        if (!outOfOrderKeys.empty()) {
            cout << "첫번째 Key: " << outOfOrderKeys.front() << endl;
            cout << "마지막 Key: " << outOfOrderKeys.back() << endl;
        } else {
            cerr  << "ERR: 해당 구간에 out of order key가 없습니다.\n";
        }
    }

    //기존 dataset에서 out of order key와 겹치는 데이터 제거
    for (int segment = 0; segment < numSegments; ++segment) {
        auto& outOfOrderKeys = outOfOrderKeysPerSegment[segment];
        auto it = dataset.begin();
        while (it != dataset.end()) {
            auto found = find(outOfOrderKeys.begin(), outOfOrderKeys.end(), it->first);
            if (found != outOfOrderKeys.end()) {
                it = dataset.erase(it); //
            } else {
                ++it;
            }
        }
    }

    // 각 구간에 out of order set을 추가
    for (int segment = 0; segment < numSegments; ++segment) {
        if (!dataset.empty()) {
            // o3 data를 추가할 randomIndex값을 사용자로부터 입력받기
            int userInputIndex;
            cout << "Enter randomIndex (" << segment+1 << "구간): ";
            cin >> userInputIndex;

            randomIndex=userInputIndex-outOfOrderKeysPerSegment[segment].size()-1;

            // outOfOrderKeys[seg]의 요소들을 input으로 받은 index위치에 추가
            for (const auto& key : outOfOrderKeysPerSegment[segment]) {
                dataset.insert(dataset.begin() + randomIndex++, make_pair(key, static_cast<int>(key * 2)));
            }

        }
    }
    tree = new DBManager();
    // 데이터셋 tree에 삽입
    cout << "최종 DataSet" << endl;
    for (const auto& pair : dataset) {
        tree->insert(pair.first, pair.second);
//        cout << pair.first << ": " << pair.second << endl;
    }
}


void DataFactory:: delayedTest(){

    tree->printActiveMemtable(false);
    tree->printImmMemtable();
    tree->Disk->printSSTableList();

    //parameter case별로 테스트 결과 달라짐
//    cout<<"\n\n\n\n========read/range test=========\n";
//
//    cout<<"[Read] key : "<<2009<<" value : "<<tree->readData(2009)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<2010<<" value : "<<tree->readData(2010)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<2011<<" value : "<<tree->readData(2011)<<"\n"; // normal ImmMemtable read
//    cout<<"[Read] key : "<<1000<<" value : "<<tree->readData(1000)<<"\n"; // normalSSTable read
//    cout<<"[Read] key : "<<1200<<" value : "<<tree->readData(1200)<<"\n"; // normalSSTable read
//    // Active에만 있는 data를 읽으려 할 때 불가! : -1
//    cout<<"[Read] key : "<<2390<<" value : "<<tree->readData(2390)<<"\n";
//
//    // 하나의 normal ImmMemtable range
//    cout<<"\n[range] 하나의 normal ImmMemtable range : 2020 ~ 2030\n";
//    map<uint64_t, int> result = tree->range(2020,2030);
//    for(auto data: result){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//    // 여러개의 normal ImmMemtable range
//    cout<<"\n[range] 여러개의 normal ImmMemtable range : 2005 ~ 2015\n";
//    map<uint64_t, int> result1 = tree->range(2005,2015);
//    for(auto data: result1){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // 한개의 SStalble range
//    cout<<"\n[range] 한개의 SStalble range : 700 ~ 710\n";
//    map<uint64_t, int> result2 = tree->range(700,710);
//    for(auto data: result2){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // 여러개의 SStalble range
//    cout<<"\n[range] 여러개의 SStalble range : 330 ~ 340\n";
//    map<uint64_t, int> result3 = tree->range(330,340);
//    for(auto data: result3){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
//
//    // ImmTable과 SStable에 섞여있을때
//    cout<<"\n[range] ImmTable과 SStable에 섞여있을때 : 1670 ~ 1680\n";
//    map<uint64_t, int> result4 = tree->range(1670,1680);
//    for(auto data: result4){
//        cout<<"key : "<<data.first<<" value : "<<data.second<<"\n";
//    }
    delete tree;
};

void DataFactory::printDelayData(){

    int delaySSTableNum= tree->Disk->delaySSTables.size();
    int delaySSTableSize=0;
    if(delaySSTableNum!= 0){
        delaySSTableSize= tree->Disk->delaySSTables.front()->ss.size();
    }

    cout<<"the number of delay data in Disk : "<<delaySSTableNum*delaySSTableSize<<"\n";

    int delayImmMemtableNum=0;
    int delayActiveMemtableNum=tree->activeDelayMemtable->mem.size();
    for(auto memtable : tree->immMemtableList){
        if(memtable->type=='D') delayImmMemtableNum++;
    }

    cout<<"the number of delay data in Memory : "<< delayImmMemtableNum*delaySSTableSize+delayActiveMemtableNum<<"\n";

}

void DataFactory::deleteAllSSTable() {
    std::string directoryPath = "../src/test/SSTable"; // SSTable 폴더의 경로
    try {
        // 디렉터리 내의 모든 파일 순회
        for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
            filesystem::remove(entry.path()); // 파일 삭제
            cout << "Deleted: " << entry.path() << endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void DataFactory::writeToFile(size_t bytes){

    ofstream file(filename, ios::binary);
    vector<char> data(bytes);

    if(!file.is_open()){
        cout<<"안녕ㄹ\n";
    }

    // 데이터를 무작위로 생성
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 255);

    for (size_t i = 0; i < bytes; ++i) {
        data[i] = static_cast<char>(distrib(gen));
    }

    auto start = chrono::high_resolution_clock::now();
    file.write(data.data(), data.size());
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsed = end - start;
    cout << "Write time: " << elapsed.count() << " ms" << endl;

    file.close();

}
void DataFactory::readFromFile(size_t bytes){

    ifstream file(filename, ios::binary);
    vector<char> data(bytes);

    auto start = chrono::high_resolution_clock::now();
    file.read(data.data(), bytes);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsed = end - start;
    cout << "Read time: " << elapsed.count() << " ms" << endl;

    file.close();

}
