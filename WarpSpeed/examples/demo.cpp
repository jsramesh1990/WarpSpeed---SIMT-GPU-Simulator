#include <iostream>
#include <vector>
#include <chrono>
#include "../src/core/block.hpp"
#include "../src/kernels/reduction.hpp"
#include "../src/utils/logger.hpp"

void performanceDemo() {
    Logger::info("=== Performance Demo ===");
    
    const std::vector<int> sizes = {64, 128, 256, 512, 1024};
    
    for (int N : sizes) {
        std::vector<int> input(N);
        for (int i = 0; i < N; i++) input[i] = 1;
        
        ThreadBlock block(N);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        int result = block.executeKernel(
            [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
                return reduction_kernel(ctx, shared, barrier, input.data(), N);
            }
        );
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        Logger::info("N=" + std::to_string(N) + 
                    ", Sum=" + std::to_string(result) +
                    ", Time=" + std::to_string(duration.count()) + "us");
    }
}

void warpDivergenceDemo() {
    Logger::info("\n=== Warp Divergence Demo ===");
    
    const int N = 64;
    ThreadBlock block(N);
    
    std::vector<int> results(N, 0);
    
    block.executeKernel(
        [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
            // Simulate warp divergence with if-else
            if (ctx.laneId % 2 == 0) {
                // Even lanes take longer path
                results[ctx.threadId] = ctx.threadId * 2;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            } else {
                // Odd lanes take short path
                results[ctx.threadId] = ctx.threadId;
            }
            
            barrier.sync();
            
            if (ctx.threadId == 0) {
                Logger::info("Warp divergence simulated: different execution paths");
            }
        }
    );
    
    Logger::info("Results computed with divergence");
}

int main() {
    Logger::setDebugEnabled(false);  // Reduce noise for demo
    Logger::setInfoEnabled(true);
    
    Logger::info("=== GPU Simulator Demo ===");
    
    // Basic examples
    {
        Logger::info("\n[Example 1] Basic Reduction");
        const int N = 8;
        std::vector<int> input(N);
        for (int i = 0; i < N; i++) input[i] = i + 1;
        
        ThreadBlock block(N);
        int sum = block.executeKernel(
            [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
                return reduction_kernel(ctx, shared, barrier, input.data(), N);
            }
        );
        
        std::cout << "Sum of 1.." << N << " = " << sum << std::endl;
    }
    
    // Performance test
    performanceDemo();
    
    // Warp divergence example
    warpDivergenceDemo();
    
    Logger::info("\n=== Demo Complete ===");
    
    return 0;
}
