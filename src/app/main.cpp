#include <atomic>
#include <condition_variable>
#include <csignal>
#include <iostream>

#include "../lib/SignalHandler.h"
#include "../lib/TcpServer.h"

using app::tcp;
using app::TcpServer;

int main() {
  app::SignalHandler signal_handler;

  boost::asio::io_context io_context;
  tcp::endpoint endpoint(tcp::v4(), 1234);

  TcpServer server(io_context, endpoint, std::thread::hardware_concurrency());
  server.start();

  io_context.run();

  signal_handler.wait_for_interrupt();

  server.stop();
  return 0;
}
