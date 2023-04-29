#include <boost/test/unit_test.hpp>
#include <libtcp_hash/util.hpp>

using namespace libtcp_hash;
using namespace std::chrono;
namespace {
BOOST_AUTO_TEST_SUITE(UtilTests, TEST_TIMEOUT_SECS(16))

BOOST_AUTO_TEST_CASE(NanoSinceEpochTest) {
  // Test nanoSinceEpoch() with allowed delta 3'000 nanoseconds
  constexpr auto delta{3'000'000};
  auto expected{duration_cast<nanoseconds>(
                    high_resolution_clock::now().time_since_epoch())
                    .count()};
  BOOST_TEST(approximatelyEqual(nanoSinceEpoch(), expected, delta));
}

BOOST_AUTO_TEST_CASE(FixedFilledWidthTest) {
  BOOST_TEST(fixedFilledWidth("", 3, '_') == "___");
  BOOST_TEST(fixedFilledWidth("1", 3, '_') == "__1");
  BOOST_TEST(fixedFilledWidth("12345", 3, '+') == "345");
}

BOOST_AUTO_TEST_CASE(AnalyzeMetricsTest) {
  LoadtestMetrics metrics;
  metrics.timestampStart = 0;
  metrics.timestampStop = 1'000;     // 1 microsecond
  metrics.bytesSent = 600'000;       //
  metrics.messagesReceived = 30'000; //
  MetricsAnalized analized = analyzeMetrics(metrics);

  BOOST_TEST(analized.megaHashesPerSecond == 30'000.);
  BOOST_TEST(analized.avgDataSize == 20.);
  BOOST_TEST(analized.seconds == 1e-6);
}

BOOST_AUTO_TEST_CASE(RandomStringTest) {
  BOOST_TEST(randomString(0, 0) == "");
  BOOST_TEST(randomString(0, 123) == "");
  BOOST_TEST(randomString(7, 123) == "\262\266Im:\260\215");
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace