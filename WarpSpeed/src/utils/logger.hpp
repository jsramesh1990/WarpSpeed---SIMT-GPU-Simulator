#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

class Logger {
private:
    static bool enableDebug;
    static bool enableInfo;
    static bool enableWarnings;
    static bool enableErrors;
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }
    
public:
    static void setDebugEnabled(bool enabled) { enableDebug = enabled; }
    static void setInfoEnabled(bool enabled) { enableInfo = enabled; }
    
    static void debug(const std::string& message) {
        if (enableDebug) {
            std::cout << "[DEBUG " << getTimestamp() << "] " << message << std::endl;
        }
    }
    
    static void info(const std::string& message) {
        if (enableInfo) {
            std::cout << "[INFO " << getTimestamp() << "] " << message << std::endl;
        }
    }
    
    static void warning(const std::string& message) {
        if (enableWarnings) {
            std::cout << "[WARNING " << getTimestamp() << "] " << message << std::endl;
        }
    }
    
    static void error(const std::string& message) {
        if (enableErrors) {
            std::cerr << "[ERROR " << getTimestamp() << "] " << message << std::endl;
        }
    }
};

// Initialize static members
bool Logger::enableDebug = true;   // Change to false in release
bool Logger::enableInfo = true;
bool Logger::enableWarnings = true;
bool Logger::enableErrors = true;
