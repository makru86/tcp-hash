#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <libtcp_hash/Server.h>

using app::tcp;
using app::tcp;

int main() {

  boost::asio::io_context io_context;
  tcp::endpoint endpoint(tcp::v4(), 1234);

  app::SimpleTcpListener server(io_context, endpoint, std::thread::hardware_concurrency());
  server.start();

  io_context.run();

  server.stop();
  return 0;
}
