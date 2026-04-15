#pragma once

struct ThreadContext {
    int threadId;      // Global thread ID within block (0..blockSize-1)
    int laneId;        // Lane ID within warp (0..31)
    int warpId;        // Warp ID (threadId / warpSize)
    int blockId;       // Block ID (for multi-block simulations)
    
    ThreadContext() : threadId(0), laneId(0), warpId(0), blockId(0) {}
    
    ThreadContext(int tid, int warpSize = 32) 
        : threadId(tid), 
          laneId(tid % warpSize),
          warpId(tid / warpSize),
          blockId(0) {}
    
    void print() const {
        printf("Thread[%d]: warp=%d, lane=%d, block=%d\n", 
               threadId, warpId, laneId, blockId);
    }
};
