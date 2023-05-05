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
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

using namespace libtcp_hash;
namespace {
BOOST_AUTO_TEST_SUITE(fffs)

BOOST_AUTO_TEST_CASE(TokenizerTest) {
  try {

    using namespace libtcp_hash;
    LOG_DEBUG("main");
    asio::io_context io_context;

    server server(io_context, 1234);

    //    std::thread t{[&](){
    //      io_context.run();
    //    }};
    io_context.run();
    //    t.join();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}

BOOST_AUTO_TEST_SUITE_END()
} // namespace
