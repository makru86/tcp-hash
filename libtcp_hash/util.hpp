#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#define TCP_HASH_LOG_MESSAGE(severity, msg)                                    \
  libtcp_hash::fixedFilledWidth(std::to_string(libtcp_hash::nanoSinceEpoch()), \
                                10, '0')                                       \
      << "_us " severity << " .."        \
      << libtcp_hash::fixedFilledWidth(__FILE__, 16, ' ') << ":"               \
      << libtcp_hash::fixedFilledWidth(std::to_string(__LINE__), 3, '0')       \
      << " " << msg << std::endl

/* LOG_DEBUG():
 * Enabled only for DEBUG build configuration.
 * Outputs to STDOUT output stream.
 */
#ifndef NDEBUG
#define LOG_DEBUG(msg)                                                         \
  { std::cout << TCP_HASH_LOG_MESSAGE("DEBUG", msg); }
#else
#define LOG_DEBUG(msg)
#endif

/* LOG_INFO():
 * Enabled for any build configuration.
 * Outputs to STDOUT output stream.
 */
#define LOG_INFO(msg)                                                          \
  { std::cout << TCP_HASH_LOG_MESSAGE("INFO ", msg); }

/* LOG_ERROR():
 * Enabled for any build configuration.
 * Outputs to STDERR output stream.
 */
#define LOG_ERROR(msg)                                                         \
  { std::cerr << TCP_HASH_LOG_MESSAGE("ERROR", msg); }

/* TEST_TIMEOUT_SECS():
 * Specify maximum allowed time to run for a unit test.
 */

#define TEST_TIMEOUT_SECS(seconds) *boost::unit_test::timeout(seconds)

namespace libtcp_hash {

/* getEnvOrValue():
 * Get environment variable by name, or, if env not set, get value.
 */
std::string getEnvOrValue(const std::string &envName,
                          const std::string &orValue);
int getEnvOrValue(const std::string &envName, int orValue);

/* readWholeFile():
 */
std::string readWholeFile(std::string_view filename);

/* approximatelyEqual():
 * Compare for equality, with fixed allowed delta.
 */
bool approximatelyEqual(size_t left, size_t rigth, size_t delta);

/* fixedFilledWidth():
 * If a text is too long, truncate it from left.
 * If it too short, fill it from left.
 * Useful for printing fields in log messages.
 */
std::string fixedFilledWidth(std::string const &text, size_t width,
                             std::string::value_type fill);

/* nanoSinceEpoch():
 * Nanoseconds since epoch.
 */
uint64_t nanoSinceEpoch();

/* to_hex_str():
 * Convert integer value to a HEX string.
 */
std::string to_hex_str(uint64_t value);

/* randomString():
 * generate random string of given length using random_device
 * uniform distribution.
 * */
std::string randomString(size_t length, unsigned int seed);

/* weakPtrDebugInfo():
 */
template <typename T> std::string weakPtrDebugInfo(std::weak_ptr<T> weakPtr) {
  std::stringstream ss;
  ss << "[this:" << weakPtr.get() << " use_count:" << weakPtr.use_count()
     << "] ";
  return ss.str();
}

} // namespace libtcp_hash