#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "../utils/logger.hpp"

class Barrier {
private:
    const int totalThreads;
    std::atomic<int> waitingThreads;
    std::atomic<int> generation;
    std::mutex mutex;
    std::condition_variable cv;
    
public:
    Barrier(int count) : totalThreads(count), waitingThreads(0), generation(0) {
        Logger::debug("Barrier created for " + std::to_string(count) + " threads");
    }
    
    void sync() {
        int myGen = generation.load(std::memory_order_acquire);
        int arrived = ++waitingThreads;
        
        if (arrived == totalThreads) {
            // Last thread to arrive - release everyone
            waitingThreads = 0;
            generation++;
            cv.notify_all();
            Logger::debug("Barrier released (all " + std::to_string(totalThreads) + " threads arrived)");
        } else {
            // Wait for release
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this, myGen] {
                return myGen != generation.load(std::memory_order_acquire);
            });
        }
    }
    
    // For debugging
    int getWaitingCount() const {
        return waitingThreads.load();
    }
};
