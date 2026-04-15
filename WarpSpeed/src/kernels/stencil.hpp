#pragma once

#include "../runtime/thread_context.hpp"
#include "../memory/shared_memory.hpp"
#include "../sync/barrier.hpp"

// 1D 3-point stencil: output[i] = (input[i-1] + input[i] + input[i+1]) / 3
inline void stencil_kernel_1d(ThreadContext& ctx, SharedMemory& shared,
                              Barrier& barrier, const float* input, 
                              float* output, int N) {
    
    // Load into shared memory with halo
    shared[ctx.threadId] = input[ctx.threadId];
    
    // Load neighbors (with boundary handling)
    if (ctx.threadId == 0) {
        // Left boundary
        shared[N] = input[0];
        shared[N + 1] = input[1];
    }
    if (ctx.threadId == N - 1) {
        // Right boundary
        shared[N + 2] = input[N - 1];
        shared[N + 3] = input[N - 2];
    }
    
    barrier.sync();
    
    // Compute stencil
    if (ctx.threadId >= 0 && ctx.threadId < N) {
        float left, center, right;
        
        if (ctx.threadId == 0) {
            left = shared.get<float>(N);      // Boundary handling
            center = shared.get<float>(0);
            right = shared.get<float>(1);
        } else if (ctx.threadId == N - 1) {
            left = shared.get<float>(N - 2);
            center = shared.get<float>(N - 1);
            right = shared.get<float>(N + 2); // Boundary handling
        } else {
            left = shared.get<float>(ctx.threadId - 1);
            center = shared.get<float>(ctx.threadId);
            right = shared.get<float>(ctx.threadId + 1);
        }
        
        output[ctx.threadId] = (left + center + right) / 3.0f;
    }
    
    barrier.sync();
}

// 2D 5-point stencil (more complex example)
inline void stencil_kernel_2d(ThreadContext& ctx, SharedMemory& shared,
                              Barrier& barrier, const float* input,
                              float* output, int width, int height) {
    
    int x = ctx.threadId % width;
    int y = ctx.threadId / width;
    
    if (x >= 0 && x < width && y >= 0 && y < height) {
        // Load current and neighboring cells
        shared[ctx.threadId] = input[y * width + x];
        
        barrier.sync();
        
        // Simple 5-point stencil
        float center = shared.get<float>(ctx.threadId);
        float up = (y > 0) ? shared.get<float>((y-1) * width + x) : center;
        float down = (y < height-1) ? shared.get<float>((y+1) * width + x) : center;
        float left = (x > 0) ? shared.get<float>(y * width + (x-1)) : center;
        float right = (x < width-1) ? shared.get<float>(y * width + (x+1)) : center;
        
        output[y * width + x] = (up + down + left + right + center) / 5.0f;
    }
}
