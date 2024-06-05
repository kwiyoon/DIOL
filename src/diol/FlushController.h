#ifndef FLUSHCONTROLLER_H
#define FLUSHCONTROLLER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <climits>
#include <mutex>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include "memtable/IMemtable.h"
#include "memtableController/ImmutableMemtableController.h"
#include "MockDisk.h"

using namespace std;
class ImmutableMemtableController;

class FlushController {
public:
    FlushController(ImmutableMemtableController& imm)
            :disk(MockDisk::getInstance()),
              taskCompleted(false), immMemtableController(imm){
    }
    // 시작 메소드
    void start(Type t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        workers.emplace_back(&FlushController::run, this, t);
    }

    // 중지 메소드

    // 작업 완료를 대기
    void waitForCompletion() {
        std::unique_lock<std::mutex> lock(m_mutex);
        condition.wait(lock, [this] { return taskCompleted; });
    }

    bool waitAndStop() {
        if(!workers.empty()){
            for (auto& worker : workers) {
                if (worker.joinable()) {
//                    worker.detach();
                    cout<<"";
                    worker.join();
                }
            }
            workers.clear();
            workers.shrink_to_fit();
        }
        condition.notify_all();
    }

    ~FlushController() { }

    void checkTimeout(Type t);
    void doFlushNoThread(Type t) {
        IMemtable *memtable = nullptr;
        // flushQueue 락
        {
            std::unique_lock<std::mutex> lock(flushQueueMutex);
//                while(true) {
            if (!immMemtableController.flushQueue.empty()) {
                memtable = immMemtableController.flushQueue.front();
            } else {
                return;
            }
            if (memtable != nullptr) {
                // memtable 락
                std::unique_lock<std::mutex> memtableLock(memtable->immMutex);
                while (memtable->memTableStatus == READING);
                memtable->memTableStatus = FLUSHING;
                if (disk.flush(memtable)) {
                    immMemtableController.flushQueue.pop();
                    lock.unlock();
                    memtableLock.unlock();
                    delete memtable;
                }
            }
        }
    }

private:
//    std::atomic<bool> running;
    vector<thread> workers;
//    std::thread worker;
    ImmutableMemtableController& immMemtableController;
    std::condition_variable condition;
    bool taskCompleted;
    std::mutex flushQueueMutex;
    mutex m_mutex;
    MockDisk& disk;
    IMemtable* findMemtableWithMinAccess(vector<IMemtable*> &v);

    void run(Type t) {
        doFlush(t);

        // 스레드 종료를 위한 clean up
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = std::find_if(workers.begin(), workers.end(), [](std::thread &worker) {
                return worker.get_id() == std::this_thread::get_id();
            });
            if (it != workers.end()) {
                if (it->joinable()) {
                    it->detach();  // 스레드를 백그라운드로 돌려서 종료 대기
//                    it->join();  // 스레드를 백그라운드로 돌려서 종료 대기
                }
                workers.erase(it);  // 벡터에서 제거
                if (workers.empty()) {
                    condition.notify_all();
                }
            }

        }
    }

    void doFlush(Type t) {
            IMemtable *memtable = nullptr;
            // flushQueue 락
            {
                std::unique_lock<std::mutex> lock(flushQueueMutex);
//                while(true) {
                    if (!immMemtableController.flushQueue.empty()) {
                        memtable = immMemtableController.flushQueue.front();
                    } else {
                        return;
                    }
                if (memtable != nullptr) {
                    // memtable 락
                    std::unique_lock<std::mutex> memtableLock(memtable->immMutex);
                    while (memtable->memTableStatus == READING);
                    memtable->memTableStatus = FLUSHING;
                    if (disk.flush(memtable)) {
                        immMemtableController.flushQueue.pop();
                        cout<<"";
                        lock.unlock();
                        memtableLock.unlock();
                        delete memtable;
                    }
                }
            }
    }
};

#endif // FLUSHCONTROLLER_H