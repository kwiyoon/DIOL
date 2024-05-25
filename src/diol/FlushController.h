#ifndef FLUSHCONTROLLER_H
#define FLUSHCONTROLLER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "IMemtable.h"
#include "memtableController/ImmutableMemtableController.h"

using namespace std;

// TODO : 스레드
class FlushController {
public:
    FlushController() : running(false) {}

    // 시작 메소드
    void start() {
        running = true;
        worker = std::thread(&FlushController::run, this);
    }

    // 중지 메소드
    void stop() {
        running = false;
        if (worker.joinable()) {
            worker.join();
        }
    }

    ~FlushController() {
        stop();
    }


private:
    std::atomic<bool> running;
    std::thread worker;

    IMemtable* findFlushMem(vector<IMemtable*>&);
    void checkTimeout();
    void run() {
        while (running) {
            checkTimeout(); // checkTimeout 메소드 호출
            this_thread::sleep_for(chrono::seconds(1)); // 1초마다 반복
        }
    }
};

#endif // FLUSHCONTROLLER_H