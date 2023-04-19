#pragma once

#include <chrono>
#include <iostream>

#ifndef NDEBUG
#define LOG_DEBUG(msg) { \
std::cout << "[" << app::Log::micros_since_epoch() << "us] [DEBUG] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl; }
#else
#define LOG_DEBUG(msg)
#endif

#define LOG_INFO(msg) { \
std::cout << "[" << app::Log::micros_since_epoch() << "us] [INFO ] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl; }

#define LOG_ERROR(msg) { \
std::cerr << "[" << app::Log::micros_since_epoch() << "us] [ERROR] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl; }

namespace app {
    class Log {
    public:
        static size_t micros_since_epoch() {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        }
    };
}