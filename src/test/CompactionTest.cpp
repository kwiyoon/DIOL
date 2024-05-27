//
// Created by 이해나 on 5/13/24.
//

#include "CompactionTest.h"

std::vector<std::pair<uint64_t, int>> CompactionTest::readFileToVector(const std::string& filename) {

    std::vector<std::pair<uint64_t, int>> data;
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: " << filename << std::endl;
        return data; // Return empty vector on failure
    }

    size_t size;
    uint64_t firstKey, lastKey;
    inputFile >> size;
    inputFile >> firstKey >> lastKey;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line

    for (size_t i = 0; i < size; ++i) {
        uint64_t first;
        int second;
        inputFile >> first >> second;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line
        data.emplace_back(first, second);
    }

    inputFile.close();
    return data;

}

void CompactionTest::loadFiles() {
    for (const auto& entry : fs::directory_iterator("../src/test/SSTable")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().string();
            std::vector<std::pair<uint64_t, int>> data = readFileToVector(filename);
            if (filename.find("NormalSStable") != std::string::npos) {
                normalSStableList.push_back(data);
            } else if (filename.find("DelaySStable") != std::string::npos) {
                delaySStableList.push_back(data);
            }
        }
    }
}

void CompactionTest::compactSSTables() {
    compactionSStableList = normalSStableList; // Initialize compactionSStableList with normalSStableList

    for (const auto& delayTable : delaySStableList) {
        for (const auto& delayPair : delayTable) {
            for (auto& compactionTable : compactionSStableList) {
                uint64_t firstKey = compactionTable.front().first;
                uint64_t lastKey = compactionTable.back().first;

                if (delayPair.first >= firstKey && delayPair.first <= lastKey) {
                    auto it = std::lower_bound(compactionTable.begin(), compactionTable.end(), delayPair,
                                               [](const std::pair<uint64_t, int>& a, const std::pair<uint64_t, int>& b) {
                                                   return a.first < b.first;
                                               });
                    compactionTable.insert(it, delayPair);
                    break;
                }
            }
        }
    }

    // Output the compacted SSTables
    int fileCounter = 0;
    for (const auto& compactionTable : compactionSStableList) {
        std::string filename = "../src/test/SSTable/CompactionSStable" + std::to_string(++fileCounter) + ".txt";
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open output file: " << filename << std::endl;
            continue;
        }

        outputFile << compactionTable.size() << "\n";  // size
        outputFile << compactionTable.front().first << "\t" << compactionTable.back().first << "\n"; // first key, last key
        for (const auto& pair : compactionTable) {
            outputFile << pair.first << "\t" << pair.second << "\n";
        }
        outputFile.close();
//        std::cout << "Data written to " << filename << std::endl;
    }
}

void CompactionTest::runCompaction(){
    auto start = std::chrono::high_resolution_clock::now();
    loadFiles();
    compactSSTables();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

//    std::cout << "Compaction test : " << elapsed.count() << " ms\n";

    return;
}