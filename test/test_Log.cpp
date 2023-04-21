#include <boost/test/unit_test.hpp>

#include "../src/lib/Log.h"

using app::Log;

static bool approximately_equal(size_t left, size_t rigth, size_t delta) {
  return (left >= rigth && left - rigth <= delta) ||
         (rigth > left && rigth - left <= delta);
}

BOOST_AUTO_TEST_SUITE(LogTestSuite)

BOOST_AUTO_TEST_CASE(test_micros_since_epoch) {
  auto now = std::chrono::high_resolution_clock::now();
  size_t expected = std::chrono::duration_cast<std::chrono::microseconds>(
                        now.time_since_epoch())
                        .count();
  size_t actual = Log::micros_since_epoch();

  BOOST_TEST(approximately_equal(expected, actual, 3));
}

BOOST_AUTO_TEST_SUITE_END()
