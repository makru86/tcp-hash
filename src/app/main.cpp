#include <iostream>
#include "../lib/TcpServer.h"

using app::TcpServer;
using tcp = TcpServer::tcp;

int main()
{
    boost::asio::io_context io_context;
    tcp::endpoint endpoint(tcp::v4(), 1234);

    TcpServer server(io_context, endpoint, std::thread::hardware_concurrency());
    server.start();

    io_context.run();

    server.stop();

    return 0;
}