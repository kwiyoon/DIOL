#ifndef FLUSHCONTROLLER_H
#define FLUSHCONTROLLER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "memtable/IMemtable.h"
#include "memtableController/ImmutableMemtableController.h"
#include "MockDisk.h"

using namespace std;
class ImmutableMemtableController;

class FlushController {
public:
    FlushController(ImmutableMemtableController& imm)
            : running(false), disk(MockDisk::getInstance()),
              taskCompleted(false), immMemtableController(imm){
    }
    // 시작 메소드
    void start(Type t) {
        running = true;
        std::lock_guard<std::mutex> lock(mutex);
        workers.emplace_back(&FlushController::run, this, t);
//        worker = std::thread(&FlushController::run, this, t);
//        flush_thread = std::thread(&FlushController::doFlush, this);
    }

    // 중지 메소드
    void stop(std::thread::id threadId) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            running = false;
        }

        auto it = find_if(workers.begin(), workers.end(),
                          [threadId](std::thread &worker) { return worker.get_id() == threadId; });
        if (it != workers.end() && it->joinable()) {
            it->join();
            workers.erase(it);  // 스레드를 벡터에서 제거
        }
    }

    // 작업 완료를 대기
    void waitForCompletion() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return taskCompleted; });
    }

    ~FlushController() {
        for (auto &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }


    void checkTimeout(Type t);

private:
    std::atomic<bool> running;
    vector<thread> workers;
//    std::thread worker;
    ImmutableMemtableController& immMemtableController;
    std::condition_variable condition;
    bool taskCompleted;
    std::mutex mutex;
    std::mutex flushQueueMutex;

    MockDisk& disk;
    IMemtable* findMemtableWithMinAccess(vector<IMemtable*> &v);

    void run(Type t){
//        if(immMemtableController.flushQueue.empty())
//            checkTimeout(t);
        doFlush(t);
        {
            std::lock_guard<std::mutex> lock(mutex);
            taskCompleted = true;
        }
        condition.notify_all();
//        stop(std::this_thread::get_id());

        // 스레드 종료를 위한 clean up
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = std::find_if(workers.begin(), workers.end(), [](std::thread &worker) {
                return worker.get_id() == std::this_thread::get_id();
            });
            if (it != workers.end()) {
                if (it->joinable()) {
                    it->detach();  // 스레드를 백그라운드로 돌려서 종료 대기
                }
                workers.erase(it);  // 벡터에서 제거
            }
        }

    }

    void doFlush(Type t) {
        IMemtable *memtable = nullptr;
        // flushQueue 락
        {
            std::unique_lock<std::mutex> lock(flushQueueMutex);
            if (!immMemtableController.flushQueue.empty()) {
                memtable = immMemtableController.flushQueue.front();
                immMemtableController.flushQueue.pop();
            }
        }

        if (memtable != nullptr) {
            // memtable 락
            std::unique_lock<std::mutex> memtableLock(memtable->mutex);
            while (memtable->memTableStatus == READING);
            memtable->memTableStatus = FLUSHING;
            if (disk.flush(memtable, t)) {
                delete memtable;
            }
        }
    }
};

#endif // FLUSHCONTROLLER_H