//
// Created by 강지윤 on 5/30/24.
//

#include "FileIOTest.h"
#include <regex>

int FileIOTest::extractHalfLinesFromFilename(const string& filePath) {
    std::regex regexPattern(R"(_c(\d+))");
    std::smatch match;
    if (std::regex_search(filePath, match, regexPattern)) {
    int lineCount = std::stoi(match[1].str());
    return lineCount / 2;
    } else {
        cerr << "ERR: 파일 이름에서 line 수를 추출할 수 없습니다: " << filePath << endl;
        return 0;
    }
}


void FileIOTest::readLines(std::ifstream& file, std::list<Record>& dataset, int linesToRead) {
    std::string line;
    int lineCount = 0;
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

list<Record> FileIOTest::readFileFromStart(const std::string &filePath, int linesToRead) {
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

list<Record> FileIOTest::readFileFromMiddle(const string& filePath, int linesToRead) {
    std::list<Record> readFileDataset;
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        cerr << "ERR: 파일을 열 수 없습니다 " << filePath << endl;
        return readFileDataset;
    }

    int skipLines = extractHalfLinesFromFilename(filePath);
    file.clear();
    file.seekg(0, ios::beg);

    string line;
    for (int i = 0; i < skipLines && getline(file, line); ++i) {
        // Skip the first half lines
    }

    readLines(file, readFileDataset, linesToRead);
    file.close();
    return readFileDataset;
}


list<Record> FileIOTest::readFile(const string& filePath) {
    list<Record> dataset;
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
//        ++lineCount;
//        if (lineCount % 5000000 == 0){
//            cout<<lineCount<<"개 읽음\n";
//        }
    }
    cout<<filePath<<" 읽어오기 끝\n";

    file.close();
    return dataset;
}

void FileIOTest::writeToFile(string filePath, list<Record>& dataset) {
    filePath = "../src/test/dataset/workload/" + filePath + ".txt";
    ofstream outputFile(filePath);
    cout<<"filePath: "<<filePath<<endl;
    if (!outputFile.is_open()) {
        cerr << "workload dataset 파일 열기 오류" << endl;
        return;
    }
    size_t i = 0;
    for (const auto& record : dataset) {
        if (record.op == "RANGE") {
            outputFile << record.op << "," << record.start_key << " " << record.end_key << std::endl;
        } else {
            outputFile << record.op << "," << record.key << std::endl;
        }

    }
    cout<<"file write완료\n\n";
    outputFile.close();
}


// Record 구조체를 ostream에 출력할 수 있도록 operator<<를 재정의
std::ostream& operator<<(std::ostream& os, const Record& record) {
    if (record.op == "RANGE") {
        os << record.op << ' ' << record.start_key << ' ' << record.end_key;
    } else {
        os << record.op << ' ' << record.key;
    }
    return os;
}

void FileIOTest::writeToFileV2(string filePath, list<Record>& dataset) {
    filePath = "../src/test/dataset/workload/" + filePath + ".txt";
    ofstream outputFile(filePath);
    cout<<"filePath: "<<filePath<<endl;
    if (!outputFile.is_open()) {
        cerr << "workload dataset 파일 열기 오류" << endl;
        return;
    }
    std::cout << "\n>> Write to Workload File Progress \n";

    std::copy(dataset.begin(), dataset.end(), std::ostream_iterator<Record>(outputFile, "\n"));

    cout<<"file write완료\n\n";
    outputFile.close();
}

