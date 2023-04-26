#include <iostream>
#include <libtcp_hash/hash.h>
#include <libtcp_hash/server.h>
#include <libtcp_hash/util.h>
#include <thread>
#include <vector>

using namespace libtcp_hash;
using namespace boost::asio;
using namespace std::chrono_literals;

/*
 * serverLoadtest():
 * Benchmarking function for the server.
 */
void serverLoadtest() {
  LoadtestConfig testConfig;
  LoadtestMetrics testMetrics;
  const auto inputData{
      randomString(testConfig.dataSize, testConfig.randomGeneratorSeed)};
  LOG_INFO("Test config:  " << testConfig);

  // Test
  testMetrics.timestampStart = nanoSinceEpoch();
  testMetrics.timestampStop = nanoSinceEpoch();

  // Report
  auto analyzed{analyzeMetrics(testMetrics)};
  LOG_INFO("Test metrics:\n" << testMetrics);
  LOG_INFO("Performance test report:\n" << analyzed);

  /*
    io_service io;
    SimpleTcpListener server{io, testConfig.tcpAddress};
    std::vector<TcpHashClient> clients;
    for (int i = 0; i < testConfig.connections; ++i) {
      clients.emplace_back(TcpHashClient{io, testConfig.tcpAddress});
    }
    std::thread ioThread{[&]() { io.run(); }};
    LOG_INFO("Test config: " << testConfig);

    // Test
    testMetrics.timestampStart = nanoSinceEpoch();
    for (auto i{0}; i != testConfig.connections; ++i) {
      auto response = clients[i].request("request");
      testMetrics.messagesSent += 1;
      testMetrics.messagesReceived += 1;
      testMetrics.bytesSent += 7;
      testMetrics.bytesReceived += response.size();
    }
    testMetrics.timestampStop = nanoSinceEpoch();

    LOG_INFO("Test metrics: " << testMetrics);
    io.stop();
    ioThread.join();
  */
}
