#include "workload.h"


//파일에 데이터 쓰기
void writeDatasetToFile(const std::string& filename, const std::vector<Record>& dataset) {
    std::ofstream outputFile(filename + ".txt");
    if (!outputFile.is_open()) {
        std::cerr << "workload dataset 파일 열기 오류" << std::endl;
        return;
    }

    for (const auto& record : dataset) {
        outputFile << record.op << "," << record.key << std::endl;
    }

    outputFile.close();
}
// 데이터셋 생성 함수
std::vector<Record> generateDataset(std::string filename, int txnFileRecordCount, double readProportion, double insertProportion) {
    srand(time(NULL));

    std::vector<Record> dataset;

    int initFileRecordCount=10; // 초기 파일 레코드 개수
    int readCount = (initFileRecordCount+txnFileRecordCount) * readProportion; // 읽기 작업 해야할 총 횟수
    int insertCount = (initFileRecordCount+txnFileRecordCount) * insertProportion; // 삽입 작업 해야할 총 횟수

    int readInserted = 0;   // 진행 된 읽기 작업 횟수
    int insertInserted = 0; // 진행 된 삽입 작업 횟수

    while (readInserted < readCount || insertInserted < insertCount) {
        //insert 작업이 read 작업보다 적거나 insert 작업이 끝날경우(루프가 종료되는 시점(insertCount못채운 경우)에서 삽입 작업을 추가해야 하는 경우 고려)
        if ((double)insertInserted / insertCount < (double)readInserted / readCount || insertInserted == insertCount) {
            // insert 작업용 레코드 추가
            Record record;
            record.key = initFileRecordCount + insertInserted + 1;
            record.op = "INSERT";
            dataset.push_back(record);
            ++insertInserted;
        } else {
            // 랜덤한 인덱스에 읽기 작업 레코드 추가
            int randomIndex = rand() % (dataset.size() + 1); // 벡터의 랜덤한 위치 선택
            Record record;
            //read key를 랜덤하게 생성
            record.key = rand() % initFileRecordCount + 1;
            //read key를 최신 데이터로 읽기 위해 생성
            //record.key = initFileRecordCount - readInserted;
            record.op = "READ";
            dataset.insert(dataset.begin() + randomIndex, record); // 랜덤한 위치에 삽입
            ++readInserted;
        }
    }

    // 파일에 데이터셋 레코드를 쓰기
    writeDatasetToFile(filename, dataset);

    return dataset;
}

//int main() {
//    int recordCount = 10; // 트랜잭션 레코드 총 개수
//    double readProportion = 0.2; // 읽기 작업 비율 (20%)
//    double insertProportion = 0.8; // 삽입 작업 비율 (80%)
//
//
//    std::vector<Record> dataset = generateDataset("workloadB", recordCount, readProportion, insertProportion);
//
//    // 생성된 데이터셋 출력
//    for (const auto& record : dataset) {
//        std::cout << "ID: " << record.key << ", Category: " << record.op << std::endl;
//    }
//
//    return 0;
//}
