cmake_minimum_required(VERSION 3.16)
project(GPUSimulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -pthread")

# Options
option(BUILD_EXAMPLES "Build example programs" ON)
option(ENABLE_LOGGING "Enable detailed logging" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Main executable
add_executable(gpu_simulator 
    src/main.cpp
)

# Core library
add_library(gpu_core STATIC
    src/core/scheduler.cpp
)

target_include_directories(gpu_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(ENABLE_LOGGING)
    target_compile_definitions(gpu_core PUBLIC ENABLE_LOGGING)
    target_compile_definitions(gpu_simulator PUBLIC ENABLE_LOGGING)
endif()

# Link pthread
target_link_libraries(gpu_simulator pthread)
target_link_libraries(gpu_core pthread)

# Examples
if(BUILD_EXAMPLES)
    add_executable(gpu_demo examples/demo.cpp)
    target_link_libraries(gpu_demo gpu_core pthread)
endif()

# Install
install(TARGETS gpu_simulator DESTINATION bin)
