#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <libtcp_hash/server.h>

using namespace libtcp_hash;

int main() {

  io_context io;
  ip::tcp::endpoint endpoint(ip::tcp::v4(), 1234);

  signal_set signals(io, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io.stop(); });

  //  SimpleTcpListener server(io_context, endpoint));
  //  server.start();
  //
  //  io_context.run();
  //
  //  server.stop();
  return 0;
}
