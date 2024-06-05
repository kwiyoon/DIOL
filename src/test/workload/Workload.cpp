#include "Workload.h"
#include <queue>

using namespace std;

int Workload::extractHalfLinesFromFilename(const string& filePath) {
    std::regex regexPattern(R"(_c(\d+))");
    std::smatch match;
    if (std::regex_search(filePath, match, regexPattern)) {
        int lineCount = std::stoi(match[1].str());
        return lineCount / 2;
    } else {
        cerr << "ERR: 파일 이름에서 줄 수를 추출할 수 없습니다: " << filePath << endl;
        return 0;
    }
}

void Workload::readLines(std::ifstream& file, std::list<Record>& dataset, int linesToRead) {
    std::string line;
    int lineCount = 0;
    cout<<"file 읽는 중\n";
    while (std::getline(file, line) && (linesToRead == -1 || lineCount < linesToRead)) {
        std::istringstream iss(line);
        std::string op;

        if (std::getline(iss, op, ',')) {
            Record record;
            record.op = op;
            if (op == "RANGE") {
                std::string startKeyStr, endKeyStr;
                if (std::getline(iss, startKeyStr, ' ') && std::getline(iss, endKeyStr)) {
                    record.start_key = std::stoull(startKeyStr);
                    record.end_key = std::stoull(endKeyStr);
                } else {
                    std::cerr << "ERR: 잘못된 형식의 RANGE 레코드입니다: " << line << std::endl;
                    continue;
                }
            } else {
                std::string keyStr;
                if (std::getline(iss, keyStr)) {
                    record.key = std::stoull(keyStr);
                } else {
                    std::cerr << "ERR: 잘못된 형식의 " << op << " 레코드입니다: " << line << std::endl;
                    continue;
                }
            }
            lineCount++;
            dataset.push_back(record);
        } else {
            std::cerr << "ERR: 잘못된 형식의 레코드입니다: " << line << std::endl;
        }

    }
}

list<Record> Workload::readFileFromStart(const std::string &filePath, int linesToRead) {
    std::list<Record> readFileDataset;
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        cerr << "ERR: 파일을 열 수 없습니다 " << filePath << endl;
        return readFileDataset;
    }
    readLines(file, readFileDataset, linesToRead);
    file.close();
    return readFileDataset;
}
list<Record> Workload::readFileWhole(const string& filePath) {
    std::list<Record> readFileDataset;
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        cerr << "ERR: 파일을 열 수 없습니다 " << filePath << endl;
        return readFileDataset;
    }
    readLines(file, readFileDataset, -1); // Read all lines
    file.close();
    return readFileDataset;
}

void Workload::executeInsertWorkload(list<Record>& dataset, int start, int end) {
    iteration = 0;
    for (const auto& record : dataset) {
        tree.insert(record.key, record.key * 2);
        iteration++;

    }

}

void Workload::executeMixedWorkload(list<Record>& dataset, int start, int end) {
    // << "Workload Mixed 작업 실행 시작\n";
    iteration=0;
    for (const auto& record : dataset) {

        if (record.op == "READ") {
            tree.readData(record.key);
        } else if (record.op == "RANGE") {
            tree.range(record.start_key, record.end_key);
        } else if (record.op == "INSERT") {
            tree.insert(record.key, record.key * 2);
        } else {
            cerr << "ERR: 잘못된 형식의 레코드입니다: " << record.op << endl;
            return;
        }
        /** 진행률 출력 */
        iteration++;

    }

    //cout << "Workload Mixed 작업 실행 끝\n";
}


void Workload::executeWorkload(list<Record>& dataset, bool isMixedWorkload) {
    // cout << "workload 실행 시작\n";
    if(!isMixedWorkload){
        executeInsertWorkload(dataset, 0, dataset.size());
        return;
    }else {
        auto start = chrono::high_resolution_clock::now();
        executeMixedWorkload(dataset, 0, dataset.size());
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> elapsed = end - start;
        cout << "Workload 실행 시간: " << elapsed.count() << " ms" << endl;
        return;
    }
    //cout << "workload 실행 끝\n";
}


//DBManager* Workload::getTree() {
//    return tree;
//}
//void Workload::cleanup() {
//    delete tree;
//    tree = nullptr;
//}


void Workload::deleteAllSSTable() {
    std::string directoryPath = "/home/haena/DBDBDeep/IoTDB-lsm/src/test/SSTable";
    try {
        // 디렉터리 내의 모든 파일 순회
        for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
            filesystem::remove(entry.path()); // 파일 삭제
            cout<<"삭제\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }
    directoryPath = "/home/haena/DBDBDeep/IoTDB-lsm/src/test/compactionSSTable";
    try {
        // 디렉터리 내의 모든 파일 순회
        for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
            filesystem::remove(entry.path()); // 파일 삭제
            cout<<"삭제\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
    }
}
/*
void Workload::makeSSTable() {

    deleteAllSSTable(); //기존 파일 삭제

    cout<<"makeSSTable\n";

    int fileCounter=0;

    string filename="/home/haena/DBDBDeep/IoTDB-lsm/src/test/SSTable/NormalSStable";

    for(auto sstable:tree->Disk->normalSSTables){

        filename="/home/haena/DBDBDeep/IoTDB-lsm/src/test/SSTable/NormalSStable"+to_string(++fileCounter) + ".txt";

        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: ??" << filename << endl;
            return;
        }

        outputFile<<sstable->ss.begin()->first<<"\t"<<sstable->ss.end()->first<<"\n"; //처음키, 마지막키
        for (const auto& pair : sstable->ss) {
            outputFile << pair.first << "\t" << pair.second << "\n";
        }

        outputFile.close();
    }

    fileCounter=0;
    


    
    for(auto sstable:tree->Disk->delaySSTables){

        filename="/home/haena/DBDBDeep/IoTDB-lsm/src/test/SSTable/DelaySStable"+to_string(++fileCounter) + ".txt";

        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: " << filename << endl;
            return;
        }

        outputFile<<sstable->ss.begin()->first<<"\t"<<sstable->ss.end()->first<<"\n"; //처음키, 마지막키
        for (const auto& pair : sstable->ss) {
            outputFile << pair.first << "\t" << pair.second << "\n";
        }

        outputFile.close();
    }

}

*/

void Workload::printDelayData(){
    MockDisk& disk = MockDisk::getInstance();
    ActiveMemtableController& actCon = ActiveMemtableController::getInstance();
    ImmutableMemtableController& immCon = ImmutableMemtableController::getInstance();
   // immCon.flushController->waitAndStop();


    int D_memory = 0;
    int D_disk= 0;
    int N_disk = 0;
    int N_memory=0;

    //delay imm 세기
    cout<<"delay immutable\n";
    
    cout<<"[ M0 ] "<<  actCon.activeDelayMemtable->mem.size() <<"\n";
    int delayImmMemtableNum=0;
    for(auto memtable : immCon.delayImmMemtableList_M1){
        delayImmMemtableNum+=memtable->mem.size();
        cout<<"[ M1 ]"<<memtable->type<<" - "<<memtable->memtableId<<" : "<< memtable->mem.size()<<"\n";
    }
    for(auto memtable : immCon.delayImmMemtableList_M2){
        delayImmMemtableNum+=memtable->mem.size();
       cout<<"[ M2 ]"<<memtable->type<<" - "<<memtable->memtableId<<" : "<< memtable->mem.size()<<"\n";
    }

    cout<<"[ M0 ] "<<  actCon.activeNormalMemtable->mem.size() <<"\n";
    N_memory+= actCon.activeNormalMemtable->mem.size();
    for(auto memtable : immCon.normalImmMemtableList_M1){
        N_memory+=memtable->mem.size();
        cout<<"[ M1 ]"<<memtable->type<<" - "<<memtable->memtableId<<" : "<< memtable->mem.size()<<"\n";
    }
    for(auto memtable : immCon.normalImmMemtableList_M2){
        N_memory+=memtable->mem.size();
       cout<<"[ M2 ]"<<memtable->type<<" - "<<memtable->memtableId<<" : "<< memtable->mem.size()<<"\n";
    }


    //flush queue 세기
    std::queue<IMemtable*> tempQueue = immCon.flushQueue;
    while (!tempQueue.empty()) {
        IMemtable* memtable = tempQueue.front();
        if(memtable->type == DI){
            D_disk+=memtable->mem.size();
        }else{
            N_disk+=memtable->mem.size();
        }
        tempQueue.pop();
    }

    cout<<"flush queue (N / D)"<<N_disk<<" / " <<D_disk<<"\n";

    //disk delay, normal 세기

    cout<<"\ndelay SSTable\n";
    int i=0;
    for( auto SSTable: disk.delaySSTables){
        
        D_disk+=SSTable->rows.size();
        cout<<"["<<++i<<"] "<<SSTable->sstableId<<": "<<SSTable->rows.size()<<"\n";
    }
    cout<<"\nnormal SSTable\n";
    i=0;
    for( auto SSTable: disk.normalSSTables){
        N_disk+=SSTable->rows.size();
        cout<<"["<<++i<<"] "<<SSTable->sstableId<<": "<<SSTable->rows.size()<<"\n";
    }


    D_memory = delayImmMemtableNum+ actCon.activeDelayMemtable->mem.size();

    cout<<"delay data in Memory : "<< D_memory<<"\n";
    cout<<"delay data in Disk : "<<D_disk<<"\n";
    cout<<"normal data in Disk : "<<N_disk<<"\n";
    cout<<"data in Disk "<<(D_disk+N_disk)<<"\n";
    cout<<"total delay: "<<D_memory+D_disk<<"\n";
    cout<<"total data: "<<D_memory+D_disk+N_disk+N_memory<<"\n";


}

//기존 test diol-------

////파일에 있던 정보를 읽어와서 데이터셋 생성
//deque<Record> Workload::readFile(const string& filePath) {
//    deque<Record> dataset;
//    ifstream file(filePath.c_str());
//    if (!file.is_open()) {
//        cerr << "ERR: 파일을 열 수 없습니다 " << filePath << endl;
//        return dataset;
//    }
//
//    string line;
//    int lineCount = 0; // 현재까지 읽은 줄 수를 추적하기 위한 변수
//
////    cout<<filePath<<" 읽어오기 시작\n";
//    while (getline(file, line)) {
//        istringstream iss(line);
//        string op;
//        if (getline(iss, op, ',')) {
//            Record record;
//            record.op = op;
//            if (op == "RANGE") {
//                string startKeyStr, endKeyStr;
//                if (getline(iss, startKeyStr, ' ') && getline(iss, endKeyStr)) {
//                    record.start_key = stoull(startKeyStr);
//                    record.end_key = stoull(endKeyStr);
//                } else {
//                    cerr << "ERR: 잘못된 형식의 RANGE 레코드입니다: " << line << endl;
//                    continue;
//                }
//            } else {
//                string keyStr;
//                if (getline(iss, keyStr)) {
//                    record.key = stoull(keyStr);
//                } else {
//                    cerr << "ERR: 잘못된 형식의 " << op << " 레코드입니다: " << line << endl;
//                    continue;
//                }
//            }
//            dataset.push_back(record);
//        } else {
//            cerr << "ERR: 잘못된 형식의 레코드입니다: " << line << endl;
//        }
//        /**진행률 출력 (전체 크기 기준으로 출력하기 어려우므로 임의의 기준으로 출력)*/
////        ++lineCount;
////        if (lineCount % 5000000 == 0){
////            cout<<lineCount<<"개 읽음\n";
////        }
//    }
////    cout<<filePath<<" 읽어오기 끝\n";
//
//    file.close();
//    return dataset;
//}
//void Workload::executeInsertWorkload(deque<Record>& dataset, int start, int end) {
////    cout << "Workload Insert 작업 실행 시작\n";
//    for (int i = start; i <= end; ++i) {
//        if (dataset[i].op == "INSERT") {
//            tree.insert(dataset[i].key, dataset[i].key * 2);
//        } else {
//            cerr << "ERR: 잘못된 형식의 레코드입니다: " << dataset[i].op << endl;
//            return;
//        }
//        /**진행률 출력*/
//        if (i != 0 && i % (end / 5) == 0) {
//            INT_LOG_PROGRESS(i, end);
//        }
//    }
////    cout << "Workload Insert 작업 실행 끝\n";
//}
//
//void Workload::executeMixedWorkload(deque<Record>& dataset, int start, int end) {
////    cout << "Workload Mixed 작업 실행 시작\n";
//    for (int i = start; i < end; ++i) {
//        if (dataset[i].op == "READ") {
//            tree.readData(dataset[i].key);
//        } else if (dataset[i].op == "RANGE") {
//            tree.range(dataset[i].start_key, dataset[i].end_key);
//        } else if (dataset[i].op == "INSERT") {
//            tree.insert(dataset[i].key, dataset[i].key * 2);
//        } else {
//            cerr << "ERR: 잘못된 형식의 레코드입니다: " << dataset[i].op << endl;
//            return;
//        }
//        /** 진행률 출력 */
//        int currentProgress = i - start + 1;
//        if (currentProgress != 0 && i % ((end-start) / 5) == 0) {
//            INT_LOG_PROGRESS(currentProgress, (end-start));
//        }
//
//
//    }
////    cout << "Workload Mixed 작업 실행 끝\n";
//}
//
//void Workload::executeWorkload(deque<Record>& dataset, int initDataNum) {
////    cout << "workload 실행 시작\n";
//
//    executeInsertWorkload(dataset, 0, initDataNum/2);
//    auto start = chrono::high_resolution_clock::now();
//    executeMixedWorkload(dataset, initDataNum/2+1, dataset.size());
//    auto end = chrono::high_resolution_clock::now();
//
////    cout << "workload 실행 끝\n";
//
//    chrono::duration<double, milli> elapsed = end - start;
//    cout << "Workload 실행 시간: " << elapsed.count() << " ms" << endl;
//}
//
//
//void Workload::deleteAllSSTable() {
//    std::string directoryPath = "../src/test/SSTable"; // SSTable 폴더의 경로
//    try {
//        // 디렉터리 내의 모든 파일 순회
//        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
//            std::filesystem::remove(entry.path()); // 파일 삭제
//        }
//    } catch (const std::filesystem::filesystem_error& e) {
//        cerr << "Error: " << e.what() << endl;
//    }
//}
//
//void Workload::makeSSTable() {
//
//    MockDisk& disk = MockDisk::getInstance();
//
//    deleteAllSSTable(); //기존 파일 삭제
//
//    int fileCounter=0;
//
//    string filename="../src/test/SSTable";
//
//    for(auto sstable:disk.normalSSTables){
//
//
//        ofstream outputFile(filename);
//        if (!outputFile.is_open()) {
//            cerr << "Failed to open output file: " << filename << endl;
//            return;
//        }
//
//        filename="/NormalSStable"+ to_string(++fileCounter) + ".txt";
//        outputFile<<sstable->rows.size()<<"\n";  //사이즈
//        outputFile<<sstable->rows.begin()->first<<"\t"<<sstable->rows.end()->first<<"\n"; //처음키, 마지막키
//        for (const auto& pair : sstable->rows) {
//            outputFile << pair.first << "\t" << pair.second << "\n";
//        }
//
//        outputFile.close();
//    }
//
//    fileCounter=0;
//
//
//
//    for(auto sstable:disk.delaySSTables){
//
//
//
//        ofstream outputFile(filename);
//        if (!outputFile.is_open()) {
//            cerr << "Failed to open output file: " << filename << endl;
//            return;
//        }
//
//        filename="/DelaySStable"+ to_string(++fileCounter) + ".txt";
//        outputFile<<sstable->rows.size()<<"\n";  //사이즈
//        outputFile<<sstable->rows.begin()->first<<"\t"<<sstable->rows.end()->first<<"\n"; //처음키, 마지막키
//        for (const auto& pair : sstable->rows) {
//            outputFile << pair.first << "\t" << pair.second << "\n";
//        }
//
//        outputFile.close();
//    }
//
//}
//
//void Workload::printDelayData(){
//
//    MockDisk& disk = MockDisk::getInstance();
//    ActiveMemtableController& actCon = ActiveMemtableController::getInstance();
//    ImmutableMemtableController& immCon = ImmutableMemtableController::getInstance();
//
//
//    int delaySSTableNum= disk.delaySSTables.size();
//    int delaySSTableNum=0;
//
//
//    for(auto sstable : disk.delaySSTables){
//
//        delaySSTableNum+=sstable->rows.size();
//
//    }
//
//    int delayImmMemtableNum=0;
//    int delayActiveMemtableNum=actCon.activeDelayMemtable->mem.size();
//    for(auto memtable : immCon.delayImmMemtableList_M1){
//        delayImmMemtableNum+=memtable->mem.size();
//    }
//    for(auto memtable : immCon.delayImmMemtableList_M2){
//       delayImmMemtableNum+=memtable->mem.size();
//    }
//
//    cout<<"delay data in Memory : "<< delayImmMemtableNum+delayActiveMemtableNum<<"\n";
//    cout<<"delay data in Disk : "<<delaySSTableNum<<"\n";
//
//
//}