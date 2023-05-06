// Test that the include file is self-contained.
#include <libtcp_hash/server.hpp>

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <libtcp_hash/client.hpp>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

namespace libtcp_hash {

struct HasherMock : std::enable_shared_from_this<HasherMock> {
  using Ptr = std::shared_ptr<HasherMock>;

  ~HasherMock() {
    LOG_DEBUG("HasherMock: times_called=" << times_called << " times_created="
                                          << times_created);
  }

  // Mock configuration
  std::chrono::milliseconds processing_time{0ms};

  // Execution statistics
  size_t times_called{0};
  size_t times_created{0};
  std::mutex mutex_;

  Ptr create() {
    {
      std::lock_guard<std::mutex> lock{mutex_};
      times_created++;
    }
    // Real implementation creates new objects, but we need a shared object
    // for testing.
    return this->shared_from_this();
  }

  template <typename OnHashCb>
  void process(std::string data, OnHashCb on_hash) {
    {
      std::lock_guard<std::mutex> lock{mutex_};
      times_called++;
    }
    if (processing_time > 0ms) {
      std::this_thread::sleep_for(processing_time);
    }
    on_hash("0123456789abcdef");
  }
};

} // namespace libtcp_hash
using namespace libtcp_hash;
using namespace std::chrono_literals;
namespace {

BOOST_AUTO_TEST_SUITE(Server2Tests)

BOOST_AUTO_TEST_CASE(StartStop) {
  auto hasher = std::make_shared<HasherMock>();
  asio::io_context io_context;
  Server server(io_context, 1234, hasher);
  std::thread io_thread{[&]() { io_context.run(); }};

  std::this_thread::sleep_for(100ms);

  io_context.stop();
  server.stop();
  io_thread.join();
}

BOOST_AUTO_TEST_CASE(ServerClientTest) {
  auto hasher{std::make_shared<HasherMock>()};
  asio::io_context io_context;
  Server server(io_context, 1234, hasher);

  size_t block_size{256};
  size_t session_count{1};
  int timeout{0}; // no timeout
  LoadtestMetrics stats{};
  auto endpoints{asio::ip::tcp::resolver{io_context}.resolve(
      asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 1234})};
  TcpHashClient client{io_context,    *endpoints, block_size,
                       session_count, timeout,    stats};

  std::thread io_thread{[&]() { io_context.run(); }};
  std::this_thread::sleep_for(100ms);

  io_context.stop();
  client.stop();
  server.stop();
  io_thread.join();
}

BOOST_AUTO_TEST_CASE(StopServerWhenClientIsRunning) {
  LOG_DEBUG("StopServerWhenClientIsRunning");
  auto hasher{std::make_shared<HasherMock>()};
  hasher->processing_time = 400ms; // 400ms per hash
  asio::io_context io_context;
  Server server(io_context, 1234, hasher);

  size_t block_size{256};
  size_t session_count{1}; // 1 session
  int timeout{0};          // no timeout
  LoadtestMetrics stats{};
  auto endpoints{asio::ip::tcp::resolver{io_context}.resolve(
      asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 1234})};
  TcpHashClient client{io_context,    *endpoints, block_size,
                       session_count, timeout,    stats};

  std::thread io_thread{[&]() { io_context.run(); }};
  std::this_thread::sleep_for(100ms); // wait 100ms before stopping

  io_context.stop();
  client.stop();
  server.stop();
  io_thread.join();

  BOOST_TEST(hasher->times_called == 1);
  BOOST_TEST(hasher->times_created == 1);
}

BOOST_AUTO_TEST_CASE(StopServerWhenClientIsRunningMultipleSessions) {
  LOG_DEBUG("StopServerWhenClientIsRunningMultipleSessions");
  auto hasher{std::make_shared<HasherMock>()};
  hasher->processing_time = 400ms; // 400ms per hash
  asio::io_context io_context;
  Server server(io_context, 1234, hasher);

  size_t block_size{256};
  size_t session_count{6}; // 6 sessions
  int timeout{0};          // no timeout
  LoadtestMetrics stats{};
  auto endpoints{asio::ip::tcp::resolver{io_context}.resolve(
      asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 1234})};
  TcpHashClient client{io_context,    *endpoints, block_size,
                       session_count, timeout,    stats};

  std::thread io_thread{[&]() { io_context.run(); }};
  std::this_thread::sleep_for(100ms); // wait 100ms before stopping

  io_context.stop();
  client.stop();
  server.stop();
  io_thread.join();

  BOOST_TEST(hasher->times_called == server.thread_count_);
  BOOST_TEST(hasher->times_created == session_count);
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
