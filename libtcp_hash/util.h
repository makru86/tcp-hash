#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#define TCP_HASH_LOG_MESSAGE(severity, msg)                                    \
  width(std::to_string(nanoSinceEpoch()), 10, '0')                             \
      << " " severity " .." << width(__FILE__, 16, ' ') << ":"                 \
      << width(std::to_string(__LINE__), 3, '0') << ": " << msg << std::endl

/*
 * TEST_TIMEOUT_SECS
 * Specify maximum allowed time to run for a unit test.
 */
#define TEST_TIMEOUT_SECS(seconds) *boost::unit_test::timeout(seconds)

/*
 * LOG_DEBUG
 * Enabled only for DEBUG build configuration.
 * Outputs to STDOUT output stream.
 */
#ifndef NDEBUG
#define LOG_DEBUG(msg)                                                         \
  { std::cout << TCP_HASH_LOG_MESSAGE("DEBUG", msg); }
#else
#define LOG_DEBUG(msg)
#endif

/*
 * LOG_INFO
 * Enabled for any build configuration.
 * Outputs to STDOUT output stream.
 */
#define LOG_INFO(msg)                                                          \
  { std::cout << TCP_HASH_LOG_MESSAGE("INFO ", msg); }

/*
 * LOG_ERROR
 * Enabled for any build configuration.
 * Outputs to STDERR output stream.
 */
#define LOG_ERROR(msg)                                                         \
  { std::cerr << TCP_HASH_LOG_MESSAGE("ERROR", msg); }

namespace libtcp_hash {

/*
 * approximatelyEqual():
 * Compare for equality, with fixed allowed delta.
 */
inline bool approximatelyEqual(size_t left, size_t rigth, size_t delta) {
  return (left >= rigth && left - rigth <= delta) ||
         (rigth > left && rigth - left <= delta);
}

/*
 * width():
 * If a text is too long, truncate it from left.
 * If it too short, fill it from left.
 * Useful for printing fields in log messages.
 */
inline std::string width(std::string const &text, size_t width,
                         std::string::value_type fill) {

  if (text.size() > width) {
    return text.substr(text.size() - width, width);
  }
  return std::string(width - text.size(), fill) + text;
}

/*
 * nanoSinceEpoch():
 * Nanoseconds since epoch.
 */
inline uint64_t nanoSinceEpoch() {
  using namespace std::chrono;
  auto now = high_resolution_clock::now().time_since_epoch();
  return duration_cast<duration<uint64_t>>(now).count();
}

/*
 * getEnvOrValue():
 * Get environment variable by name, or, if env not set, get value.
 */
std::string getEnvOrValue(const std::string &envName,
                          const std::string &orValue) {
  const char *envValue = std::getenv(envName.c_str());
  if (envValue) {
    return std::string(envValue);
  }
  return orValue;
}
int getEnvOrValue(const std::string &envName, int orValue) {
  const char *envValue = std::getenv(envName.c_str());
  if (envValue) {
    return std::stoi(envValue);
  }
  return orValue;
}

} // namespace libtcp_hash