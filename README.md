README.md
markdown

# GPU Simulator in C++

A complete, educational GPU architecture simulator that models CUDA-like execution model with threads, warps, blocks, shared memory, and barriers.

## 🚀 Features

- **Thread-level parallelism** - Simulates GPU threads using std::thread
- **Warp abstraction** - Groups threads into warps (SIMT execution)
- **Thread blocks** - Multiple warps with shared memory
- **Barrier synchronization** - Models `__syncthreads()`
- **Shared memory** - Fast on-chip memory per block
- **GPU-like kernels** - Reduction, Stencil, Scan examples

## 🏗️ Architecture

Host (CPU) → ThreadBlock → Warps → Threads → Kernel Execution
↓
Shared Memory + Barrier
text


## 📦 Building

```bash
mkdir build && cd build
cmake ..
make

🎯 Running
bash

# Main simulator
./gpu_simulator

# Demo with examples
./gpu_demo

📚 Examples
Reduction Kernel
cpp

auto reduction = [](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
    shared[ctx.threadId] = input[ctx.threadId];
    barrier.sync();
    
    for (int stride = N/2; stride > 0; stride /= 2) {
        if (ctx.threadId < stride) {
            shared[ctx.threadId] += shared[ctx.threadId + stride];
        }
        barrier.sync();
    }
};

🔬 Key Concepts

    SIMT - Single Instruction Multiple Threads

    Warp - Group of 32 threads executing together

    Barrier - Synchronization point within block

    Shared Memory - User-managed cache

📊 Performance Tips

    Minimize barrier usage

    Avoid warp divergence

    Coalesce shared memory access

    Bank conflict awareness

🎓 Learning Outcomes

    Understand GPU execution model

    Learn parallel programming patterns

    Experience SIMT architecture

    Master synchronization primitives

📜 License

MIT License - See LICENSE file
🤝 Contributing

PRs welcome! Focus on:

    Adding more kernels

    Cycle-accurate simulation

    Memory coalescing visualization

    Performance counters
