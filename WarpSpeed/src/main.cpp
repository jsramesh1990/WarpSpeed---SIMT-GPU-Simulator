#include <iostream>
#include <vector>
#include "core/block.hpp"
#include "kernels/reduction.hpp"
#include "kernels/stencil.hpp"
#include "kernels/scan.hpp"
#include "utils/logger.hpp"

int main() {
    Logger::info("=== GPU Simulator Starting ===");
    
    // Test 1: Reduction kernel
    {
        Logger::info("\n[Test 1] Reduction Kernel");
        const int N = 16;
        std::vector<int> input(N);
        for (int i = 0; i < N; i++) input[i] = i + 1;
        
        ThreadBlock block(N);
        int result = block.executeKernel(
            [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
                reduction_kernel(ctx, shared, barrier, input.data(), N);
            }
        );
        
        Logger::info("Reduction result: " + std::to_string(result));
        Logger::info("Expected sum: " + std::to_string(N * (N + 1) / 2));
    }
    
    // Test 2: Stencil kernel
    {
        Logger::info("\n[Test 2] Stencil Kernel (1D 3-point)");
        const int N = 32;
        std::vector<float> input(N), output(N);
        for (int i = 0; i < N; i++) input[i] = static_cast<float>(i);
        
        ThreadBlock block(N);
        block.executeKernel(
            [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
                stencil_kernel_1d(ctx, shared, barrier, input.data(), output.data(), N);
            }
        );
        
        Logger::info("Stencil result (middle 5 values):");
        for (int i = N/2-2; i <= N/2+2; i++) {
            std::cout << "  output[" << i << "] = " << output[i] << std::endl;
        }
    }
    
    // Test 3: Scan kernel
    {
        Logger::info("\n[Test 3] Prefix Scan Kernel");
        const int N = 16;
        std::vector<int> input(N), output(N);
        for (int i = 0; i < N; i++) input[i] = 1;
        
        ThreadBlock block(N);
        block.executeKernel(
            [&](ThreadContext& ctx, SharedMemory& shared, Barrier& barrier) {
                scan_kernel(ctx, shared, barrier, input.data(), output.data(), N);
            }
        );
        
        Logger::info("Scan results (prefix sums):");
        for (int i = 0; i < N; i++) {
            std::cout << "  output[" << i << "] = " << output[i];
            if (i == N-1) std::cout << " (total)";
            std::cout << std::endl;
        }
    }
    
    Logger::info("\n=== GPU Simulator Completed Successfully ===");
    return 0;
}
