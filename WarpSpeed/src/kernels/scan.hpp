#pragma once

#include "../runtime/thread_context.hpp"
#include "../memory/shared_memory.hpp"
#include "../sync/barrier.hpp"

// Prefix sum (scan) kernel - inclusive
inline void scan_kernel(ThreadContext& ctx, SharedMemory& shared,
                       Barrier& barrier, const int* input, 
                       int* output, int N) {
    
    // Load into shared memory
    shared[ctx.threadId] = input[ctx.threadId];
    barrier.sync();
    
    // Up-sweep (reduce)
    for (int stride = 1; stride < N; stride *= 2) {
        int index = (ctx.threadId + 1) * stride * 2 - 1;
        if (index < N) {
            int sum = shared.get<int>(index) + shared.get<int>(index - stride);
            shared[index] = sum;
        }
        barrier.sync();
    }
    
    // Clear the last element
    if (ctx.threadId == N - 1) {
        shared[N - 1] = 0;
    }
    barrier.sync();
    
    // Down-sweep
    for (int stride = N / 2; stride > 0; stride /= 2) {
        int index = (ctx.threadId + 1) * stride * 2 - 1;
        if (index < N) {
            int temp = shared.get<int>(index);
            shared[index] = shared.get<int>(index) + shared.get<int>(index - stride);
            shared[index - stride] = temp;
        }
        barrier.sync();
    }
    
    // Write results
    output[ctx.threadId] = shared.get<int>(ctx.threadId);
    barrier.sync();
}

// Exclusive scan (prefix sum where output[i] = sum of input[0..i-1])
inline void exclusive_scan_kernel(ThreadContext& ctx, SharedMemory& shared,
                                 Barrier& barrier, const int* input,
                                 int* output, int N) {
    
    // First compute inclusive scan
    scan_kernel(ctx, shared, barrier, input, output, N);
    barrier.sync();
    
    // Shift to exclusive (output[i] = output[i-1], output[0] = 0)
    if (ctx.threadId == 0) {
        output[0] = 0;
    } else {
        int prev = output[ctx.threadId - 1];
        output[ctx.threadId] = prev;
    }
    barrier.sync();
}
