#include <iostream>
#include <libtcp_hash/client.hpp>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/server.hpp>
#include <libtcp_hash/util.hpp>
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
  //  io_service serverIo;
  //  EchoProtocol echoHandler;
  //  SimpleTcpListener server{serverIo, testConfig.tcpAddress, echoHandler};
  //  std::thread serverThread{[&]() { serverIo.run(); }};
  io_service clientIo;

  LOG_INFO(testConfig);

  // Test

  testMetrics.timestampStart = nanoSinceEpoch();
  {
    TcpHashClient client{clientIo, testConfig.tcpAddress, 4096, 10, 1};
    clientIo.run();
  }
  testMetrics.timestampStop = nanoSinceEpoch();

  //  serverIo.stop();
  //  serverThread.join();

  LOG_INFO(testMetrics.makeReport());

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
