#include <iostream>
#include <libtcp_hash/hash.h>
#include <libtcp_hash/util.h>
#include <thread>
#include <vector>

using namespace libtcp_hash;
using namespace boost::asio;
using namespace std::chrono_literals;

/*
 * hashLoadtest():
 */
void hashLoadtest() {
  LoadtestConfig testConfig;
  std::vector<std::thread> threads;
  LoadtestMetrics testMetrics;
  const auto inputData{
      randomString(testConfig.dataSize, testConfig.randomGeneratorSeed)};
  LOG_INFO("Test config:  " << testConfig);

  // Test
  testMetrics.timestampStart = nanoSinceEpoch();
  // For each connection...
  for (auto tid{0}; tid != testConfig.connections; ++tid) {
    // create a thread that...
    threads.emplace_back([&]() {
      // will setup once...
      XxHash xxHash{};
      FSM<XxHash> fsm{xxHash};
      // and for the given number of iterations will repeatedly...
      for (auto i{0}; i != testConfig.repeatDataIterations; ++i) {
        // call the tokenizer with the input data...
        tokenizer(inputData, //
                  [&](CharArray chunk, bool separatorFound) {
                    // on each callback from tokenizer feed FSM...
                    fsm.feed(chunk);
                    if (separatorFound) {
                      // and if the \n is found, get HashValue.
                      HashValue hash = fsm.digest();
                      testMetrics.messagesReceived += 1;
                      testMetrics.bytesReceived +=
                          sizeof(hash) + 1; // +1 for separator
                    }
                  });
        testMetrics.messagesSent += 1;
        testMetrics.bytesSent += inputData.size();
      }
    });
  }
  for (auto i{0}; i != testConfig.connections; ++i) {
    threads[i].join();
  }
  testMetrics.timestampStop = nanoSinceEpoch();

  // Report
  auto analyzed{analyzeMetrics(testMetrics)};
  LOG_INFO("Test metrics:\n" << testMetrics);
  LOG_INFO("Performance test report:\n" << analyzed);
}