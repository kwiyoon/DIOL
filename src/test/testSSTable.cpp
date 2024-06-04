#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cstdint>

class testSSTable {
public:
    std::string filename;
    uint64_t minKey;
    uint64_t maxKey;
    std::vector<std::pair<uint64_t, int>> data;

    testSSTable(const std::string& fname) : filename(fname) {
        std::ifstream file(fname);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + fname);
        }
        file >> minKey >> maxKey;
    }

    void load() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        uint64_t key;
        int value;
        while (file >> key >> value) {
            data.emplace_back(key, value);
        }
        file.close();
    }

    void save(const std::string& outname) const {
        std::ofstream file(outname);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + outname);
        }
        file << minKey << "\t" << maxKey << "\n";
        for (const auto& kv : data) {
            file << kv.first << "\t" << kv.second << "\n";
        }
        file.close();
    }
};