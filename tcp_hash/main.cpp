#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <libtcp_hash/Server.h>

using namespace libtcp_hash;

int main() {

  io_context io_context;
  ip::tcp::endpoint endpoint(tcp::v4(), 1234);

  SimpleTcpListener server(io_context, endpoint));
  server.start();

  io_context.run();

  server.stop();
  return 0;
}
