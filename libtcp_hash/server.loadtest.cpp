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
    TcpHashClient client{clientIo,   testConfig.tcpAddress, 4096, 10, 1,
                         testMetrics};
    clientIo.run();
  }
  testMetrics.timestampStop = nanoSinceEpoch();

  //  serverIo.stop();
  //  serverThread.join();

  LOG_INFO(testMetrics.makeReport());
}
