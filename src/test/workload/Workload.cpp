#include "Workload.h"

using namespace std;

//파일에 있던 정보를 읽어와서 데이터셋 생성
vector<Record> Workload::readFile(const string& filePath) {
    vector<Record> dataset;
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        cerr << "ERR: 파일을 열 수 없습니다 " << filePath << endl;
        return dataset;
    }

    string line;
    int lineCount = 0; // 현재까지 읽은 줄 수를 추적하기 위한 변수

    cout<<filePath<<" 읽어오기 시작\n";
    while (getline(file, line)) {
        istringstream iss(line);
        string op;
        if (getline(iss, op, ',')) {
            Record record;
            record.op = op;
            if (op == "RANGE") {
                string startKeyStr, endKeyStr;
                if (getline(iss, startKeyStr, ' ') && getline(iss, endKeyStr)) {
                    record.start_key = stoull(startKeyStr);
                    record.end_key = stoull(endKeyStr);
                } else {
                    cerr << "ERR: 잘못된 형식의 RANGE 레코드입니다: " << line << endl;
                    continue;
                }
            } else {
                string keyStr;
                if (getline(iss, keyStr)) {
                    record.key = stoull(keyStr);
                } else {
                    cerr << "ERR: 잘못된 형식의 " << op << " 레코드입니다: " << line << endl;
                    continue;
                }
            }
            dataset.push_back(record);
        } else {
            cerr << "ERR: 잘못된 형식의 레코드입니다: " << line << endl;
        }
        /**진행률 출력 (전체 크기 기준으로 출력하기 어려우므로 임의의 기준으로 출력)*/
        ++lineCount;
        if (lineCount % 5000000 == 0){
            cout<<lineCount<<"개 읽음\n";
        }
    }
    cout<<filePath<<" 읽어오기 끝\n";

    file.close();
    return dataset;
}
void Workload::executeInsertWorkload(vector<Record>& dataset, int start, int end) {
    cout << "Workload Insert 작업 실행 시작\n";
    for (int i = start; i <= end; ++i) {
        if (dataset[i].op == "INSERT") {
            tree.insert(dataset[i].key, dataset[i].key * 2);
        } else {
            cerr << "ERR: 잘못된 형식의 레코드입니다: " << dataset[i].op << endl;
            return;
        }
        /**진행률 출력*/
        if (i != 0 && i % (end / 100) == 0) {
            INT_LOG_PROGRESS(i, end);
        }
    }
    cout << "Workload Insert 작업 실행 끝\n";
}

void Workload::executeMixedWorkload(vector<Record>& dataset, int start, int end) {
    cout << "Workload Mixed 작업 실행 시작\n";
    for (int i = start; i < end; ++i) {
        if (dataset[i].op == "READ") {
            tree.readData(dataset[i].key);
        } else if (dataset[i].op == "RANGE") {
            tree.range(dataset[i].start_key, dataset[i].end_key);
        } else if (dataset[i].op == "INSERT") {
            tree.insert(dataset[i].key, dataset[i].key * 2);
        } else {
            cerr << "ERR: 잘못된 형식의 레코드입니다: " << dataset[i].op << endl;
            return;
        }
        /** 진행률 출력 */
        int currentProgress = i - start + 1;
        if (currentProgress != 0 && i % ((end-start) / 100) == 0) {
            INT_LOG_PROGRESS(currentProgress, (end-start));
        }


    }
    cout << "Workload Mixed 작업 실행 끝\n";
}

void Workload::executeWorkload(vector<Record>& dataset, int initDataNum) {
    cout << "workload 실행 시작\n";

    executeInsertWorkload(dataset, 0, initDataNum/2);
    auto start = chrono::high_resolution_clock::now();
    executeMixedWorkload(dataset, initDataNum/2+1, dataset.size());
    auto end = chrono::high_resolution_clock::now();

    cout << "workload 실행 끝\n";

    chrono::duration<double, milli> elapsed = end - start;
    cout << "Workload 실행 시간: " << elapsed.count() << " ms" << endl;
}


void Workload::deleteAllSSTable() {
    std::string directoryPath = "../src/test/SSTable"; // SSTable 폴더의 경로
    try {
        // 디렉터리 내의 모든 파일 순회
        for (const auto& entry : std::__fs::filesystem::directory_iterator(directoryPath)) {
            std::__fs::filesystem::remove(entry.path()); // 파일 삭제
        }
    } catch (const std::__fs::filesystem::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void Workload::makeSSTable() {

    deleteAllSSTable(); //기존 파일 삭제

    int fileCounter=0;

    string filename="../src/test/SSTable/NormalSStable";

    MockDisk& disk = MockDisk::getInstance();
    for(auto sstable:disk.normalSSTables){

        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: " << filename << endl;
            return;
        }

        filename+=  to_string(++fileCounter) + ".txt";
        outputFile<<sstable->rows.size()<<"\n";  //사이즈
        outputFile<<sstable->rows.begin()->first<<"\t"<<sstable->rows.end()->first<<"\n"; //처음키, 마지막키
        for (const auto& pair : sstable->rows) {
            outputFile << pair.first << "\t" << pair.second << "\n";
        }

        outputFile.close();
    }

    fileCounter=0;

    filename="../src/test/SSTable/DelaySStable";

    for(auto sstable:disk.delaySSTables){

        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: " << filename << endl;
            return;
        }

        filename+=  to_string(++fileCounter) + ".txt";
        outputFile<<sstable->rows.size()<<"\n";  //사이즈
        outputFile<<sstable->rows.begin()->first<<"\t"<<sstable->rows.end()->first<<"\n"; //처음키, 마지막키
        for (const auto& pair : sstable->rows) {
            outputFile << pair.first << "\t" << pair.second << "\n";
        }

        outputFile.close();
    }

}

/*void Workload::printDelayData(){
    MockDisk& disk = MockDisk::getInstance();

    int delaySSTableNum= tree.Disk->delaySSTables.size();
    int delaySSTableSize=0;
    if(delaySSTableNum!= 0){
        delaySSTableSize= tree.Disk->delaySSTables.front()->ss.size();
    }

    cout<<"the number of delay data in Disk : "<<delaySSTableNum*delaySSTableSize<<"\n";

    int delayImmMemtableNum=0;
    int delayActiveMemtableNum=tree.activeDelayMemtable->mem.size();
    for(auto memtable : tree.immMemtableList){
        if(memtable->type=='D') delayImmMemtableNum++;
    }

    cout<<"the number of delay data in Memory : "<< delayImmMemtableNum*delaySSTableSize+delayActiveMemtableNum<<"\n";

}
 */