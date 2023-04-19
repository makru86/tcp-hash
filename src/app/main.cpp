#include <iostream>
#include <condition_variable>
#include <csignal>
#include <atomic>

#include "../lib/TcpServer.h"
#include "../lib/SignalHandler.h"

using app::tcp;
using app::TcpServer;

int main()
{
    app::SignalHandler signal_handler;

    boost::asio::io_context io_context;
    tcp::endpoint endpoint(tcp::v4(), 1234);

    TcpServer server(io_context, endpoint, std::thread::hardware_concurrency());
    server.start();

    signal_handler.wait_for_interrupt();

    server.stop();
    return 0;
}