#include <iostream>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
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
  LOG_INFO(testConfig);

  // Test
  testMetrics.timestampStart = nanoSinceEpoch();
  // For each session...
  for (auto tid{0}; tid != testConfig.clientSessions; ++tid) {
    // create a thread that...
    threads.emplace_back([&]() {
      // will setup once...
      XxHash xxHash{};
      StatefulHasher<XxHash> fsm{xxHash};
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
  for (auto i{0}; i != testConfig.clientSessions; ++i) {
    threads[i].join();
  }
  testMetrics.timestampStop = nanoSinceEpoch();

  // Report
  auto analyzed{analyzeMetrics(testMetrics)};
  LOG_INFO(testMetrics);
  LOG_INFO(analyzed);
}