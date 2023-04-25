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
#include <libtcp_hash/server.h>
#include <vector>

using namespace libtcp_hash;
namespace {

std::vector<uint8_t> message_to_send;

uint64_t nanoSinceEpoch() {
  using namespace std::chrono;
  auto now = high_resolution_clock::now().time_since_epoch();
  return duration_cast<duration<uint64_t>>(now).count();
}

template <typename T> T environment(const std::string &name, const T &def) {
  const char *value = std::getenv(name.c_str());
  if (std::is_same<T, std::string>::value) {
    return value ? value : def;
  }
  if (std::is_same<T, int>::value) {
    return value ? std::stoi(value) : def;
  }
  return value ? value : def;
}

std::atomic<uint64_t> timestamp_start(nanoSinceEpoch());
std::atomic<uint64_t> timestamp_stop(nanoSinceEpoch());

std::atomic<uint64_t> total_errors(0);
std::atomic<uint64_t> total_bytes(0);
std::atomic<uint64_t> total_messages(0);

static void loadtest(benchmark::State &state) {

  //  // Client parameters
  //  std::string address("localhost");
  //  int port = 1234;
  //  int connections_count = environment<int>("LOADTEST_CONNECTIONS", 100);
  //  int seconds_count = environment<int>("LOADTEST_SECONDS", 10);
  //
  //  std::cout << "Server address: " << address << std::endl;
  //  std::cout << "Server port: " << port << std::endl;
  //  std::cout << "Concurrent connections: " << connections_count << std::endl;
  //  std::cout << "Seconds to benchmarking: " << seconds_count << std::endl;
  //
  //  std::cout << std::endl;
  //
  //  // Create echo clients
  //  std::vector<app::TcpHashClient> clients;
  //  for (int i = 0; i < clients_count; ++i)
  //  {
  //    // Create echo client
  //    auto client = app::TcpH(service, address, port, messages_count);
  //    // client->SetupNoDelay(true);
  //    clients.emplace_back(client);
  //  }
  //
  //
  //  std::set<int> data;
  //  for (auto _ : state) {
  //    state.PauseTiming();
  //    data = app::ConstructRandomSet(state.range(0));
  //    state.ResumeTiming();
  //    for (int j = 0; j < state.range(1); ++j)
  //      data.insert(app::RandomNumber());
  //  }
}

} // namespace

BENCHMARK(loadtest)
    ->Args({1 << 8, 128})
    ->Args({2 << 8, 128})
    ->Args({1 << 8, 512})
    ->Args({2 << 8, 512});

BENCHMARK_MAIN();
