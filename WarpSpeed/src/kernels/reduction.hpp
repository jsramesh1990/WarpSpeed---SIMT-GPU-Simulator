#pragma once

#include "../runtime/thread_context.hpp"
#include "../memory/shared_memory.hpp"
#include "../sync/barrier.hpp"

// Parallel reduction kernel of your sys
inline int reduction_kernel(ThreadContext& ctx, SharedMemory& shared, 
                           Barrier& barrier, const int* input, int N) {
    
    // Load input into shared memory
    if (ctx.threadId < N) {
        shared[ctx.threadId] = input[ctx.threadId];
    } else {
        shared[ctx.threadId] = 0;
    }
    
    barrier.sync();  // Wait for all loads to complete
    
    // Perform reduction
    for (int stride = N / 2; stride > 0; stride /= 2) {
        if (ctx.threadId < stride) {
            int sum = shared.get<int>(ctx.threadId) + 
                     shared.get<int>(ctx.threadId + stride);
            shared[ctx.threadId] = sum;
        }
        barrier.sync();  // Wait for all threads in this stride
    }
    
    // Thread 0 returns the result
    if (ctx.threadId == 0) {
        return shared.get<int>(0);
    }
    
    return 0;
}

// Alternative: using atomic operations (more realistic) & PLEASE check tha file is present or not
#include <atomic>

inline int reduction_kernel_atomic(ThreadContext& ctx, SharedMemory& shared,
                                   Barrier& barrier, std::atomic<int>* input, int N) {
    // Each thread adds its value atomically
    if (ctx.threadId < N) {
        // This simulates atomicAdd in CUDA
        int old = shared.get<int>(0);
        while (!shared[0].compare_exchange_weak(old, old + input[ctx.threadId])) {}
    }
    
    barrier.sync();
    
    if (ctx.threadId == 0) {
        return shared.get<int>(0);
    }
    
    return 0;
}
