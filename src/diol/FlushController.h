#ifndef FLUSHCONTROLLER_H
#define FLUSHCONTROLLER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "IMemtable.h"
#include "memtableController/ImmutableMemtableController.h"
#include "MockDisk.h"

using namespace std;

// TODO : 스레드
class FlushController {
public:
    FlushController() : running(false), disk(MockDisk::getInstance()),
                        immMemtableController(ImmutableMemtableController::getInstance()) {}

    // 시작 메소드
    void start() {
        running = true;
        timeout_thread = std::thread(&FlushController::checkTimeoutLoop, this);
        flush_thread = std::thread(&FlushController::doFlushLoop, this);
    }

    // 중지 메소드
    void stop() {
        running = false;
        if (timeout_thread.joinable()) {
            timeout_thread.join();
        }
        if (flush_thread.joinable()) {
            flush_thread.join();
        }
    }

    ~FlushController() {
        stop();
    }


private:
    std::atomic<bool> running;
    std::thread timeout_thread;
    std::thread flush_thread;
    ImmutableMemtableController& immMemtableController;
    mutex mtx;
    MockDisk& disk;

    IMemtable* findFlushMem(vector<IMemtable*>&);
    void checkTimeout();

    void checkTimeoutLoop() {
        while (running) {
            checkTimeout();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void doFlushLoop() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);
            cout<<"FlushController::doFlushLoop\n";
            if (immMemtableController.flushQueue.empty() && !running) {
                break;
            }

            if (!immMemtableController.flushQueue.empty()) {
                cout<<"FlushController::doFlushLoop - notEmpty\n";

                IMemtable *memtable = immMemtableController.flushQueue.front();
                immMemtableController.flushQueue.pop();
                lock.unlock();

                disk.flush(memtable);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

#endif // FLUSHCONTROLLER_H