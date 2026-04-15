#pragma once

#include <functional>
#include "../runtime/thread_context.hpp"
#include "../memory/shared_memory.hpp"
#include "../sync/barrier.hpp"

using Kernel = std::function<void(ThreadContext&, SharedMemory&, Barrier&)>;
