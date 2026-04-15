#pragma once

#include <vector>
#include <string>
#include "../utils/logger.hpp"

class WarpGroup {
private:
    int warpId;
    std::vector<int> threadIds;
    
public:
    WarpGroup(int id, const std::vector<int>& ids) 
        : warpId(id), threadIds(ids) {}
    
    void executeSIMT(std::function<void(int)> instruction) {
        // Simulate SIMT execution - all threads execute same instruction
        for (int threadId : threadIds) {
            instruction(threadId);
        }
    }
    
    void print() const {
        std::string threads;
        for (int tid : threadIds) {
            threads += std::to_string(tid) + " ";
        }
        Logger::debug("  Warp " + std::to_string(warpId) + 
                     " (threads: " + threads + ")");
    }
    
    int getWarpId() const { return warpId; }
    const std::vector<int>& getThreadIds() const { return threadIds; }
    int getNumThreads() const { return threadIds.size(); }
};
