#pragma once

#include <vector>
#include <cstring>
#include <type_traits>
#include "../utils/logger.hpp"

class SharedMemory {
private:
    std::vector<char> memory;
    size_t size;
    
public:
    // For integer/float storage
    template<typename T>
    class Proxy {
    private:
        char* ptr;
    public:
        Proxy(char* p) : ptr(p) {}
        
        operator T() const {
            T value;
            std::memcpy(&value, ptr, sizeof(T));
            return value;
        }
        
        T operator=(const T& value) {
            std::memcpy(ptr, &value, sizeof(T));
            return value;
        }
        
        T operator+=(const T& value) {
            T current = *this;
            T result = current + value;
            *this = result;
            return result;
        }
    };
    
    SharedMemory(size_t elements = 0, size_t elementSize = sizeof(int)) 
        : memory(elements * elementSize), size(elements) {
        Logger::debug("SharedMemory allocated: " + std::to_string(elements) + " elements");
    }
    
    template<typename T>
    Proxy<T> operator[](size_t index) {
        if (index >= size) {
            throw std::out_of_range("Shared memory access out of bounds");
        }
        return Proxy<T>(memory.data() + index * sizeof(T));
    }
    
    template<typename T>
    const T get(size_t index) const {
        if (index >= size) return T();
        T value;
        std::memcpy(&value, memory.data() + index * sizeof(T), sizeof(T));
        return value;
    }
    
    template<typename T>
    void set(size_t index, const T& value) {
        if (index >= size) return;
        std::memcpy(memory.data() + index * sizeof(T), &value, sizeof(T));
    }
    
    size_t getSize() const { return size; }
    
    void clear() {
        std::memset(memory.data(), 0, memory.size());
    }
    
    // For debugging
    template<typename T>
    void print() const {
        std::cout << "SharedMemory contents: ";
        for (size_t i = 0; i < size; i++) {
            std::cout << get<T>(i) << " ";
        }
        std::cout << std::endl;
    }
};
