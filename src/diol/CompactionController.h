#ifndef COMPACTIONCONTROLLER_H
#define COMPACTIONCONTROLLER_H

#include "memtable/IMemtable.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

class CompactionController {
public:
    CompactionController() : running(false) {}

    void checkTimeOut();

    // 시작 메소드
    void start() {
        running = true;
        worker = std::thread(&CompactionController::run, this);
    }

    // 중지 메소드
    void stop() {
        running = false;
        if (worker.joinable()) {
            worker.join();
        }
    }
private:
    std::atomic<bool> running;

    std::thread worker;
    IMemtable* findCompactionMem();
    void run() {
        checkTimeOut(); // checkTimeout 메소드 호출
        stop();
    }
};

#endif // COMPACTIONCONTROLLER_H
