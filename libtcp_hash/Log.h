#pragma once

#include <chrono>
#include <iostream>

#define TCP_HASH_LOG_MESSAGE(severity, msg)                                    \
  "[" << libtcp_hash::Log::micros_since_epoch() << "us] [" severity "] "       \
      << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl

#ifndef NDEBUG
#define LOG_DEBUG(msg)                                                         \
  { std::cout << TCP_HASH_LOG_MESSAGE("DEBUG", msg); }
#else
#define LOG_DEBUG(msg)
#endif

#define LOG_INFO(msg)                                                          \
  { std::cout << TCP_HASH_LOG_MESSAGE("INFO ", msg); }

#define LOG_ERROR(msg)                                                         \
  { std::cerr << TCP_HASH_LOG_MESSAGE("ERROR", msg); }

namespace libtcp_hash {
class Log {
public:
  static size_t micros_since_epoch() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(
               now.time_since_epoch())
        .count();
  }
};
} // namespace libtcp_hash