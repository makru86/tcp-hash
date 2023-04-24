#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>

// Helper for LOG macros
#define TCP_HASH_LOG_MESSAGE(severity, msg)                                    \
  width(std::to_string(micros_since_epoch()), 10, '0')                         \
      << " " severity " .." << width(__FILE__, 16, ' ') << ":"                 \
      << width(std::to_string(__LINE__), 3, '0') << ": " << msg << std::endl

// Helper for unit tests
#define TEST_TIMEOUT_SECS(seconds) *boost::unit_test::timeout(seconds)

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

/*
 * micros_since_epoch():
 * Returns the number of microseconds since epoch.
 */
inline size_t micros_since_epoch() {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(
             now.time_since_epoch())
      .count();
}

// Helper for unittests, to compare with fixed allowed delta.
bool approximatelyEqual(size_t left, size_t rigth) {
  constexpr size_t delta{3};
  return (left >= rigth && left - rigth <= delta) ||
         (rigth > left && rigth - left <= delta);
}

/*
 * width():
 * If a text is too long, truncate it from left.
 * If it too short, fill it from left.
 * Useful for printing fields in log messages.
 */
std::string width(std::string const &text, size_t width,
                  std::string::value_type fill) {

  if (text.size() > width) {
    return text.substr(text.size() - width, width);
  }
  return std::string(width - text.size(), fill) + text;
}

} // namespace libtcp_hash