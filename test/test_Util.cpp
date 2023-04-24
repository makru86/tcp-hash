#include <boost/test/unit_test.hpp>
#include <libtcp_hash/Util.h>

using namespace libtcp_hash;
using namespace std::chrono;
namespace {

BOOST_AUTO_TEST_SUITE(UtilTests, TEST_TIMEOUT_SECS(16))

BOOST_AUTO_TEST_CASE(test_micros_since_epoch) {
  auto timeSinceEpoch{high_resolution_clock::now().time_since_epoch()};
  auto expected{duration_cast<microseconds>(timeSinceEpoch).count()};

  auto actual{micros_since_epoch()};
  BOOST_TEST(approximatelyEqual(expected, actual));
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace