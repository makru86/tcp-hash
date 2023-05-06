// Test that the include file is self-contained.
#include <libtcp_hash/server2.hpp>

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/test/unit_test.hpp>
#include <compare>
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

using namespace libtcp_hash;
using namespace std::chrono_literals;
namespace {

struct HasherMock {
  using UPtr = std::unique_ptr<HasherMock>;

  UPtr clone() {
    times_hasher_cloned++;
    return std::make_unique<HasherMock>();
  }

  template <typename OnHashCb>
  void process(std::string data, OnHashCb on_hash) {
    times_called_process++;
    if (processing_time > 0ms) {
      std::this_thread::sleep_for(processing_time);
    }
    on_hash("0123456789abcdef");
  }

  std::chrono::milliseconds processing_time{0ms};
  size_t times_called_process{0};
  size_t times_hasher_cloned{0};
};

BOOST_AUTO_TEST_SUITE(Server2Tests)

BOOST_AUTO_TEST_CASE(StartStop) {
  auto hasher = std::make_unique<HasherMock>();
  asio::io_context io_context;
  Server server(io_context, 1234, std::move(hasher));
  std::thread io_thread{[&]() { io_context.run(); }};

  std::this_thread::sleep_for(100ms);

  io_context.stop();
  server.stop();
  io_thread.join();
}

BOOST_AUTO_TEST_CASE(ServerClientTest) {
  auto hasher{std::make_unique<HasherMock>()};
  asio::io_context io_context;
  Server server(io_context, 1234, std::move(hasher));

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
  auto hasher{std::make_unique<HasherMock>()};
  asio::io_context io_context;
  Server server(io_context, 1234, std::move(hasher));

  size_t block_size{256};
  size_t session_count{1}; // 1 session
  int timeout{0};          // no timeout
  LoadtestMetrics stats{};
  auto endpoints{asio::ip::tcp::resolver{io_context}.resolve(
      asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 1234})};
  TcpHashClient client{io_context,    *endpoints, block_size,
                       session_count, timeout,    stats};

  hasher->processing_time = 400ms; // 400ms per hash
  std::thread io_thread{[&]() { io_context.run(); }};
  std::this_thread::sleep_for(100ms); // wait 100ms before stopping

  io_context.stop();
  client.stop();
  server.stop();
  io_thread.join();

  BOOST_TEST(hasher->times_called_process > 0);
  BOOST_TEST(hasher->times_hasher_cloned == 1);
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
