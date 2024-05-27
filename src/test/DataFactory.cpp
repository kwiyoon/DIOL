

#include "DataFactory.h"


using namespace std;

//o3 데이터 없이 데이터셋 생성
void DataFactory:: generateNormalDataset(int n){
    for (uint64_t i = 1; i <= n; ++i) {
        auto data = make_pair(i, static_cast<int>(i * 2));
        tree.insert(data.first, data.second);
    }
}


/** DelayData 포함 Data 생성 함수*/
void DataFactory:: generateDelayedDataset(string& dataSetName, int dataNum, double outOfOrderRatio) {
    int outOfOrderCount = static_cast<int>(dataNum * outOfOrderRatio); // out of order 데이터 총 개수
    int segmentDataNum= outOfOrderCount/2;
    int numSegments= segmentDataNum/20;  //하이퍼파라미터
    int iteration = 0; // 진행률 표시를 위한 변수

    std::vector<int> indices(segmentDataNum);
    std::iota(indices.begin(), indices.end(), 0);

    // Randomly shuffle the indices
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    std::vector<std::vector<int>> outOfOrderKeysPerSegment(numSegments); //segment 묶음 단위 out of order 데이터들을 저장할 벡터


//    srand(time(0));
//    random_device rd; // 난수 생성기 시드
//    mt19937 gen(rd()); // Mersenne Twister 난수 생성기


    /**1~dataNum 범위 dataset 초기화
     * */
    vector<uint64_t> dataSet(dataNum);

    cout<< "dataSet 1부터 dataNum까지 초기화\n";
    std::iota(dataSet.begin(), dataSet.end(), 1);
    cout<< "dataSet 초기화 완료\n";



    /**out of order data 각 segment 크기 설정
     * */
    cout<<"생성되어야할 segment 개수 : "<<numSegments<<"\n";
    int count25Percent = numSegments * 0.25;
    for (int i = 0; i < count25Percent; ++i) {
        sizes.push_back(10);
    }

    for (int i = 0; i < count25Percent; ++i) {
        sizes.push_back(30);
    }

    int count50Percent = numSegments * 0.50;
    for (int i = 0; i < count50Percent; ++i) {
        sizes.push_back(20);
    }

    //생성되는 segment 개수가 1:1:2로 정수로 나누어지지 않는 경우 계산 보정
    if(count25Percent+count25Percent+count50Percent!=numSegments){
        sizes.push_back(20);
    }

    std::shuffle(sizes.begin(), sizes.end(), g);



    /**out of order data segment에 들어갈 key 선정
     * */
    int distance = dataNum / numSegments; // 각 delay segment 생성될 범위 조정 (distance구간당 segment 1개씩 매핑)
    int start=1;
    int end=distance;
    for (size_t i = 0; i < sizes.size(); i++) {
        std::uniform_int_distribution<> nextDis(start, end-sizes[i]); // currentIdx 부터 distance-(생성될 segment 사이즈) 사이에서 seg에 들어갈 첫번째 delay key선정
        int delayedKey = nextDis(g);

        std::cout << "\n구간 " << i + 1 << " (" << sizes[i] << "개) : " << delayedKey << " ~ " << delayedKey + sizes[i] - 1 << "\n";

        start = end+1;
        end = start+distance-1;

        for (int j = 0; j < sizes[i]; j++) {
            outOfOrderKeysPerSegment[i].push_back(delayedKey + j);
        }


    }



    /**out of order segment에 포함되는 key들 찾아 dataSet에서 제거
     * */
    unordered_set<uint64_t> outOfOrderKeys;
    for (const auto& segment : outOfOrderKeysPerSegment) {
        for (const auto& key : segment) {
            outOfOrderKeys.insert(key);
        }
    }

    dataSet.erase(
            remove_if(dataSet.begin(), dataSet.end(), [&](uint64_t key) {
                return outOfOrderKeys.find(key) != outOfOrderKeys.end();
            }),
            dataSet.end()
    );


    /**out of order 단일 key 선정
     * */
    vector<uint64_t> remainingKeys(dataSet.begin(), dataSet.end());
    srand(static_cast<unsigned>(std::time(0))); // 난수 생성기 초기화

    std::unordered_set<uint64_t> randomKeys;
    size_t numberOfKeysToSelect = outOfOrderCount / 2;

    while (randomKeys.size() < numberOfKeysToSelect) {
        size_t randomIndex = rand() % remainingKeys.size();
        randomKeys.insert(remainingKeys[randomIndex]);
    }

    dataSet.erase(
            remove_if(dataSet.begin(), dataSet.end(), [&](uint64_t key) {
                return randomKeys.find(key) != randomKeys.end();
            }),
            dataSet.end()
    );


    /** dataset에 단일 out of order data 추가
     * */
    cout<<"\n>> 단일 Out of order data 추가\n\n";
    for(const auto &key : randomKeys) {
        int randomChoice = rand() % 100 + 1;
        if (randomChoice <= 30) {
            randomIndex = key + (rand() % 500 + 1); // 1~500 범위 내
        } else if (randomChoice <= 60) {
            randomIndex = key + (rand() % 500 + 501); // 501~1000 범위 내
        } else if (randomChoice <= 80) {
            randomIndex = key + (rand() % 1000 + 1001); // 1001~2000 범위 내
        } else {
            randomIndex = rand() % dataSet.size() + 1 + key;
        }

        // 랜덤 인덱스가 dataSet 범위를 벗어나면 dataSet 뒤에 추가
        if(randomIndex >= dataSet.size()){
            dataSet.push_back(key);
        }else{
            dataSet.insert(dataSet.begin() + randomIndex, key);
        }

        iteration++;
//        cout<<randomKeys.size()<<endl;
        if ( iteration % (randomKeys.size() / 10) == 0) {
            VECTOR_LOG_PROGRESS(iteration, randomKeys);
        }

    }


    /** dataset에 out of order data segment 추가
     * */
    cout<<">> Out of order dataSet 추가\n\n";
    iteration=0;
    for(const auto &segment : outOfOrderKeysPerSegment){
        //현재 segment의 마지막 key값보다 큰 인덱스에 랜덤 생성(offset)
        int randomChoice = rand() % 100 + 1;
        int isIndexValid;
        if(segment.back()==0){
            cerr << "ERR: delay segment size생성이 제대로 되지 않았습니다. 총 segment 개수와 0.25, 0.25, 0.5 비율";
        }
        if (randomChoice <= 30) {
            randomIndex = rand() % 500 + 1; // 1~500 범위 내
            isIndexValid = segment.back() + randomIndex;
        } else if (randomChoice <= 60) {
            randomIndex = rand() % 500 + 501; // 501~1000 범위 내
            isIndexValid = segment.back() + randomIndex;
        } else if (randomChoice <= 80) {
            randomIndex = rand() % 1000 + 1001; // 1001~2000 범위 내
            isIndexValid = segment.back()  + randomIndex;
        } else {
            randomIndex = rand() % dataSet.size() + 1; // 1 ~ dataSet.size() 범위 내
            isIndexValid = segment.back() + randomIndex;
        }

        // 랜덤 인덱스가 dataSet 범위를 벗어나면 dataSet 뒤에 추가
        if(isIndexValid>= dataSet.size()){
            for (const auto& key : segment) {
                dataSet.push_back(key);
            }
        }
        else{
            for (const auto& key : segment) {
                dataSet.insert(dataSet.begin() + isIndexValid++, key);

            }
        }
        iteration++;
        if ( iteration % (outOfOrderKeysPerSegment.size() / 10) == 0) {
            INT_LOG_PROGRESS(iteration, outOfOrderKeysPerSegment.size());
        }
    }


    /** File에 쓰기
    * */
    string filePath = "../src/test/dataset/"+dataSetName+ "_c"+to_string(dataNum)+"_d"+to_string(outOfOrderRatio).substr(0, std::to_string(outOfOrderRatio).find('.') + 2)+".txt";
    if (!dataSet.empty()) {
        writeToInitFile(filePath, dataSet);
    } else {
        cerr << "ERR:"<<filePath <<"가 비어있습니다." << endl;
    }
}

void DataFactory::writeToInitFile(string filePath, vector<uint64_t>& dataset) {
    ofstream outputFile(filePath);
    std::cout << "\n>> Write to Init File Progress \n\n";

    if (!outputFile.is_open()) {
        cerr << "ERR: workload dataset 파일 열기 오류" << endl;
        return;
    }

    for(int i=0; i<dataset.size(); i++){
        outputFile << "INSERT," << dataset[i] << endl;
        if (i != 0 && i % (dataset.size() / 100) == 0) {
            VECTOR_LOG_PROGRESS(i, dataset);
        }
    }

    outputFile.close();
}



/**File에 Workload 쓰기 함수*/
void DataFactory::writeToWorkloadFile(string filePath, vector<Record>& dataset) {
    ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        cerr << "workload dataset 파일 열기 오류" << endl;
        return;
    }
    std::cout << "\n>> Write to Workload File Progress \n";
    for(int i=0; i<dataset.size(); i++){
        if (strcmp(dataset[i].op.c_str(), "RANGE")==0) {
            outputFile << dataset[i].op << "," << dataset[i].start_key << " " << dataset[i].end_key << endl;
        } else {
            outputFile << dataset[i].op << "," << dataset[i].key << endl;
        }

        if (i != 0 && i % (dataset.size() / 100) == 0) {
            VECTOR_LOG_PROGRESS(i, dataset);
        }
    }
    cout<<"file write완료\n\n";
    outputFile.close();
}
std::string to_string_with_precision(double value, int precision) {
    std::ostringstream out;
    out.precision(precision);
    out << std::fixed << value;
    return out.str();
}

/** Workload 데이터 생성 함수*/
void DataFactory::generateWorkloadDataset(vector<Record>& initDataSet, string& workloadDataName, double readProportion, double insertProportion, double singleReadProportion, double rangeProportion) {

    vector<Record> dataset;
    int initFileRecordCount = initDataSet.size();   //전체 데이터셋 개수
    int txnFileRecordCount = initFileRecordCount/2; // 워크로드의 INSERT 작업 개수(전체 데이터 셋의 절반)
    int singleReadCount = txnFileRecordCount * (readProportion/insertProportion) * singleReadProportion; // 단일 읽기 작업 해야할 총 횟수
    int rangeCount = txnFileRecordCount * (readProportion/insertProportion) * rangeProportion; // 범위 조회 작업 해야할 총 횟수
    cout << ">> Generate to Workload Dataset Progress \n\n";
    cout<<"1) workload Insert 작업 추가\n";
    for(int i=0; i<initFileRecordCount; i++){
        Record record;
        record.key = initDataSet[i].key;
        record.op = "INSERT";
        dataset.push_back(record);
        if (i != 0 && i % (initFileRecordCount / 100) == 0) {
            INT_LOG_PROGRESS(i, initFileRecordCount);
        }
    }


    int totalWorkCount = singleReadCount + rangeCount;
    int completedWorkCount = 0;
    cout<<"2) Workload read, range 작업 추가\n";
    while (singleReadCount > 0 || rangeCount > 0) {
        // 랜덤한 인덱스에 read 작업 레코드 추가
        int randomReadKey = rand() % (initFileRecordCount/2) + 1;
        Record record;
        if (singleReadCount > 0) {                          // single read 작업일 경우
            record.key = randomReadKey;
            --singleReadCount;
            record.op = "READ";
        } else {                                            // range read 작업일 경우
            int rangeStart = rand() % (initFileRecordCount/2) + 1;
            int rangeEnd = rand() % (initFileRecordCount/2) + 1;
            if (rangeStart > rangeEnd) {
                swap(rangeStart, rangeEnd);
            }
            record.start_key = rangeStart;
            record.end_key = rangeEnd;
            record.op = "RANGE";
            --rangeCount;
        }
        // 워크로드 삽입
        dataset.insert(dataset.begin() + initFileRecordCount/2 + randomReadKey, record);
        completedWorkCount++;

        /**진행률 출력*/
        if (completedWorkCount % (totalWorkCount / 100) == 0) {
            INT_LOG_PROGRESS(completedWorkCount, totalWorkCount);
        }
    }

    /**파일에 쓰기*/
    std::string filePath;
    if(singleReadProportion == 0.5) {
        filePath = "../src/test/dataset/"+workloadDataName+"_i" + to_string_with_precision(insertProportion, 1) +
                   "_i" + to_string_with_precision(readProportion, 1) + "_V1.txt";
    } else {
        filePath = "../src/test/dataset/"+workloadDataName+"_r"+ to_string_with_precision(readProportion, 1) +
                   "_i" + to_string_with_precision(insertProportion, 1) + "_V2.txt";
    }
    writeToWorkloadFile(filePath, dataset);

    return;
}



void DataFactory::writeToFile(size_t bytes){

    ofstream file(filename, ios::binary);
    vector<char> data(bytes);

    if(!file.is_open()){
        cerr << "ERR: 파일 open 오류\n";
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