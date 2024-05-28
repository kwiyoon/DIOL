//
// Created by 나경 on 2024/05/11.
//

#ifndef IOTDB_DBManager_WORKLOAD_H
#define IOTDB_DBManager_WORKLOAD_H

#include "../DataFactory.h"
#include "../../diol/DBManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <sstream>
#include <filesystem>
#include <regex>


class Workload {
public:
    vector<int> op;    //INSERT: 0, READ: 1, RANGE: 2
    deque<uint64_t> workloadKey;
    deque<pair<uint64_t, uint64_t> > workloadRangeKey;
    DBManager& tree;
    MockDisk& disk;



    Workload(DBManager& _tree, MockDisk& _disk) : tree(_tree), disk(_disk) {}
    Workload(const Workload&) = delete;
    deque<Record> readFile(const string& filePath);
    void executeInsertWorkload(deque<Record>& dataset, int start, int end);
    void executeMixedWorkload(deque<Record>& dataset, int start, int end);
    void executeWorkload(deque<Record>& dataset, int initDataNum);
    void cleanup();
    DBManager* getTree();
    void deleteAllSSTable();
    void makeSSTable();
    void printDelayData();

};
#endif //IOTDB_LSM_WORKLOAD_H