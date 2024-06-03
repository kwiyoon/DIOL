//
// Created by 강지윤 on 5/30/24.
//

#ifndef GITIGNORE_FILEIOTEST_H
#define GITIGNORE_FILEIOTEST_H

//#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
//#include <chrono>
//#include <unordered_set>
#include <sstream>
//#include <algorithm>
//#include <cstring>
#include <deque>
#include "DataFactory.h"

using namespace std;

class FileIOTest {
public:
    list<Record> readFile(const string& filePath);
    void readLines(std::ifstream& file, std::list<Record>& dataset, int linesToRead);
    list<Record> readFileFromStart(const string& filePath, int linesToRead);
    list<Record> readFileFromMiddle(const string& filePath, int linesToRead);

    void writeToFile(string filePath, list<Record>& dataset);
    void writeToFileV2(string filePath, list<Record>& dataset);
    int extractHalfLinesFromFilename(const string& filePath);
};


#endif //GITIGNORE_FILEIOTEST_H
