////
//// Created by 이해나 on 5/13/24.
////
//
//#ifndef IOTDB_LSM_COMPACTIONTEST_H
//#define IOTDB_LSM_COMPACTIONTEST_H
//
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <algorithm>
//#include <chrono>
//#include <utility>
//#include <filesystem> // C++17부터 사용 가능
//#include <list>
//
//using namespace std;
//namespace fs = std::filesystem;
//
//class CompactionTest {
//
//public:
//
//    std::list<std::vector<std::pair<uint64_t, int>>> normalSStableList;
//    std::list<std::vector<std::pair<uint64_t, int>>> delaySStableList;
//    std::list<std::vector<std::pair<uint64_t, int>>> compactionSStableList;
//
//    std::vector<std::pair<uint64_t, int>> readFileToVector(const std::string& filename);
//    void loadFiles();
//    void compactSSTables();
//    void runCompaction();
//
//};
//
//
//#endif //IOTDB_LSM_COMPACTIONTEST_H
