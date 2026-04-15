#pragma once

#include <queue>
#include <vector>
#include <memory>
#include "block.hpp"

class GPUScheduler {
private:
    std::vector<std::unique_ptr<ThreadBlock>> blocks;
    int currentBlock;
    
public:
    GPUScheduler() : currentBlock(0) {}
    
    void addBlock(std::unique_ptr<ThreadBlock> block) {
        blocks.push_back(std::move(block));
    }
    
    void executeAll() {
        Logger::info("GPU Scheduler: Executing " + std::to_string(blocks.size()) + " blocks");
        
        for (auto& block : blocks) {
            // In real GPU, blocks execute concurrently
            // Here we simulate sequential execution
            Logger::debug("  Executing block " + std::to_string(currentBlock++));
        }
    }
    
    void reset() {
        currentBlock = 0;
    }
};
