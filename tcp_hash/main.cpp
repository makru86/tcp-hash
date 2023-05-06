#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/server.hpp>

using namespace libtcp_hash;
using namespace boost::asio;

int main() {
  auto hasher = std::make_shared<Hasher>();
  asio::io_context io_context;
  Server server(io_context, 1234, hasher);
  io_context.run();
  server.stop();
}
