#ifndef COMPACTIONCONTROLLER_H
#define COMPACTIONCONTROLLER_H

#include "memtable/IMemtable.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <climits>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <vector>

#define LOG_STR(str) \
    cout<<str<<endl;

class CompactionController {
public:
    CompactionController() : running(false), taskCompleted(false) {}

    void checkTimeOut();

    // 시작 메소드
    void start() {
        running = true;
        taskCompleted = false;
        worker = std::thread(&CompactionController::run, this);
    }

    // 중지 메소드
    void stop() {
        running = false;
        if (worker.joinable()) {
            worker.join();
        }
    }

    // 작업 완료를 대기
    void waitForCompletion() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return taskCompleted; });
    }

private:
    std::atomic<bool> running;
    std::thread worker;
    std::mutex mutex;
    std::condition_variable condition;
    bool taskCompleted;

    IMemtable* findMemtableWithMinDelay();
    void run() {
        checkTimeOut(); // checkTimeout 메소드 호출
        {
            std::lock_guard<std::mutex> lock(mutex);
            taskCompleted = true;
        }
        condition.notify_all();
    }
};

#endif // COMPACTIONCONTROLLER_H
