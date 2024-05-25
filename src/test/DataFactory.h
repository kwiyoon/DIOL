
#ifndef dataFactory_H
#define dataFactory_H
using namespace std;
class DataFactory {
public:
    void generateNormalDataset(int n);
    void NormalTest();
    void generateDelayedDataset(int n, double outOfOrderRatio, int numSegments);
    void delayedTest();
    void printDelayData();
    void deleteAllSSTable();
    //N바이트 쓸때 시간 측정
    void writeToFile(size_t bytes);
    void readFromFile(size_t bytes);
private:
    DBManager* tree;
    int outOfOrderCount;    //o3 data 몇개인지 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    uint64_t randomIndex;  //o3 data가 삽입되는 인덱스 (o3데이터 어디에 있는지 위치 계산할 때 필요)
    string filename="dump.txt";
};

#endif //dataFactory_H
