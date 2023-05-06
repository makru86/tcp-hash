#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <libtcp_hash/server.hpp>

using namespace libtcp_hash;
using namespace boost::asio;

int main() {
  io_service io;
  ip::tcp::endpoint endpoint(ip::tcp::v4(), 1234);
  signal_set signals(io, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io.stop(); });
  return 0;
}
