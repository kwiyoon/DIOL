//
// Created by 나경 on 2024/05/11.
//

#ifndef IOTDB_LSM_WORKLOAD_H
#define IOTDB_LSM_WORKLOAD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

struct Record {
    std::string op;
    int key;
};

void writeDatasetToFile(const std::string& filename, const std::vector<Record>& dataset);
std::vector<Record> generateDataset(std::string filename, int txnFileRecordCount, double readProportion, double insertProportion);

#endif //IOTDB_LSM_WORKLOAD_H

