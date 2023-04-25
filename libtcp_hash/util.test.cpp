#include <boost/test/unit_test.hpp>
#include <libtcp_hash/util.h>

using namespace libtcp_hash;
using namespace std::chrono;
namespace {
BOOST_AUTO_TEST_SUITE(UtilTests, TEST_TIMEOUT_SECS(16))

BOOST_AUTO_TEST_CASE(NanoSinceEpochTest) {
  auto timeSinceEpoch{high_resolution_clock::now().time_since_epoch()};
  auto expected{duration_cast<nanoseconds>(timeSinceEpoch).count()};
  auto actual{nanoSinceEpoch()};

  // Test equal with allowed delta 3'000 nanoseconds.
  BOOST_TEST(approximatelyEqual(expected, actual, 3000));
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace