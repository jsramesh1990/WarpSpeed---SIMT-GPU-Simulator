#pragma once

#include <vector>
#include <thread>
#include <memory>
#include "warp.hpp"
#include "../runtime/thread_context.hpp"
#include "../memory/shared_memory.hpp"
#include "../sync/barrier.hpp"
#include "../utils/logger.hpp"

class ThreadBlock {
private:
    int blockSize;
    int warpSize;
    std::vector<WarpGroup> warps;
    std::vector<std::thread> cpuThreads;
    std::vector<ThreadContext> contexts;
    SharedMemory sharedMemory;
    Barrier barrier;
    
    void createWarps() {
        int numWarps = (blockSize + warpSize - 1) / warpSize;
        warps.clear();
        
        for (int w = 0; w < numWarps; w++) {
            std::vector<int> threadIds;
            int startId = w * warpSize;
            int endId = std::min(startId + warpSize, blockSize);
            
            for (int tid = startId; tid < endId; tid++) {
                threadIds.push_back(tid);
            }
            
            warps.emplace_back(w, threadIds);
        }
    }
    
public:
    ThreadBlock(int size, int wSize = 32) 
        : blockSize(size), warpSize(wSize), 
          sharedMemory(size), barrier(size) {
        
        // Create thread contexts
        contexts.reserve(blockSize);
        for (int i = 0; i < blockSize; i++) {
            contexts.emplace_back(i, warpSize);
        }
        
        createWarps();
        
        Logger::debug("Created ThreadBlock with " + std::to_string(blockSize) + 
                     " threads, " + std::to_string(warps.size()) + " warps");
    }
    
    template<typename KernelFunc>
    typename std::invoke_result_t<KernelFunc, ThreadContext&, SharedMemory&, Barrier&>
    executeKernel(KernelFunc kernel) {
        
        using ReturnType = typename std::invoke_result_t<KernelFunc, ThreadContext&, SharedMemory&, Barrier&>;
        
        // For kernels that return a value (like reduction)
        if constexpr (!std::is_same_v<ReturnType, void>) {
            std::vector<ReturnType> results(blockSize);
            std::vector<std::thread> threads;
            
            // Launch threads
            for (int i = 0; i < blockSize; i++) {
                threads.emplace_back([&, i]() {
                    results[i] = kernel(contexts[i], sharedMemory, barrier);
                });
            }
            
            // Join all threads
            for (auto& t : threads) {
                t.join();
            }
            
            // Return result from thread 0 (or combine as needed)
            return results[0];
        }
        // Void kernel (no return value)
        else {
            std::vector<std::thread> threads;
            
            for (int i = 0; i < blockSize; i++) {
                threads.emplace_back([&, i]() {
                    kernel(contexts[i], sharedMemory, barrier);
                });
            }
            
            for (auto& t : threads) {
                t.join();
            }
            
            return;
        }
    }
    
    void printWarpInfo() const {
        Logger::info("ThreadBlock Configuration:");
        Logger::info("  Total threads: " + std::to_string(blockSize));
        Logger::info("  Warp size: " + std::to_string(warpSize));
        Logger::info("  Total warps: " + std::to_string(warps.size()));
        
        for (const auto& warp : warps) {
            warp.print();
        }
    }
    
    SharedMemory& getSharedMemory() { return sharedMemory; }
    Barrier& getBarrier() { return barrier; }
};
