#ifndef COMPACTIONCONTROLLER_H
#define COMPACTIONCONTROLLER_H

#include "memtable/IMemtable.h"
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

// TODO : 스레드
class CompactionController {
public:
    CompactionController() : running(false) {}
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
    atomic<bool> running;
    thread worker;

    IMemtable* findCompactionMem();
    void checkTimeOut();
    void run() {
        while (running) {
            checkTimeOut(); // checkTimeout 메소드 호출
            this_thread::sleep_for(chrono::seconds(10)); // 5초마다 반복
        }
    }
};


#endif // COMPACTIONCONTROLLER_H
