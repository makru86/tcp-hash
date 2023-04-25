/*
 * loadtest.cpp
 * Performance testing for the tcp_hash application.
 *
 * Inspired by chronoxor/CppServer benchmark:
 * https://github.com/chronoxor/CppServer/blob/master/performance/tcp_echo_client.cpp
 *
>>> Server address: 127.0.0.1
>>> Server port: 1111
>>> Working clients: 100
>>> Working messages: 1000
>>> Message size: 32
>>> Seconds to benchmarking: 10
>>>
>>> Errors: 0
>>>
>>> Total time: 10.007 s
>>> Total data: 1.151 GiB
>>> Total messages: 38503396
>>> Data throughput: 117.423 MiB/s
>>> Message latency: 259 ns
>>> Message throughput: 3847402 msg/s
 */

#include <atomic>
#include <benchmark/benchmark.h>
#include <chrono>
#include <iostream>
#include <libtcp_hash/hash.h>
#include <libtcp_hash/server.h>
#include <libtcp_hash/util.h>
#include <vector>

using namespace libtcp_hash;
using namespace boost::asio;
namespace {

/*
 * Metrics: loadtest metrics.
 */
struct Metrics {
  std::atomic<uint64_t> timestampStart{nanoSinceEpoch()};
  std::atomic<uint64_t> timestampStop{nanoSinceEpoch()};
  std::atomic<uint64_t> totalErrors{0};
  std::atomic<uint64_t> totalBytes{0};
  std::atomic<uint64_t> totalMessages{0};
};

/*
 * Config: loadtest configuration parameters.
 */
struct Config {
  int concurrentConnections_{getEnvOrValue("LOADTEST_CONNECTIONS", 100)};
  int testingTime_{getEnvOrValue("LOADTEST_SECONDS", 10)};
  ip::tcp::endpoint tcpAddress_{ip::tcp::v4(), 1234};
};
std::ostream &operator<<(std::ostream &os, const Config &config) {
  return os << "loadtest configuration:"
            << "\n Concurrent client connections: "
            << config.concurrentConnections_
            << "\n Testing time: " << config.testingTime_ << " seconds"
            << "\n Server TCP address" << config.tcpAddress_ << "\n";
}

Config testConfig{};
Metrics testMetrics{};
std::vector<uint8_t> messageToSend;

/*
 * serverLoadtest():
 * Benchmarking function for the server.
 */
void serverLoadtest(benchmark::State &bench) {
  LOG_DEBUG("Test setup started");
  LOG_DEBUG("Test config: " << testConfig);
  io_service io;
  SimpleTcpListener server{io, testConfig.tcpAddress_};
  std::vector<TcpHashClient> clients;
  for (int i = 0; i < testConfig.concurrentConnections_; ++i) {
    clients.emplace_back(TcpHashClient{io, testConfig.tcpAddress_});
  }
  std::thread ioThread{[&]() { io.run(); }};
  LOG_DEBUG("Test setup completed");

  // test
  for (auto _ : bench) {
    bench.PauseTiming();
    bench.ResumeTiming();
    for (int i = 0; i != bench.range(1); ++i)
      clients[1].request("echo Hello\n");
  }

  LOG_DEBUG("Test teardown started");
  io.stop();
  ioThread.join();
  LOG_DEBUG("Test teardown completed");
}

} // namespace

BENCHMARK(serverLoadtest)
    ->Args({1 << 8, 128})
    ->Args({2 << 8, 128})
    ->Args({1 << 8, 512})
    ->Args({2 << 8, 512});

BENCHMARK_MAIN();
