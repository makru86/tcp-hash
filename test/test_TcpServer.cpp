#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

#include "../src/lib/TcpServer.h"

using app::tcp;
using app::TcpServer;

BOOST_AUTO_TEST_CASE(test_handle_connection)
{
    boost::asio::io_context io_context;
    tcp::endpoint endpoint(tcp::v4(), 1234);

    TcpServer server(io_context, endpoint, std::thread::hardware_concurrency());

    tcp::socket socket(io_context);
    socket.connect(endpoint);

    std::string message = "World\n";
    boost::asio::write(socket, boost::asio::buffer(message));

    io_context.run();

    std::string expected_response = "Hello, World\n";
    boost::asio::streambuf response_buf;
    boost::asio::read_until(socket, response_buf, "\n");
    std::string actual_response = boost::asio::buffer_cast<const char*>(response_buf.data());

    BOOST_CHECK_EQUAL(actual_response, expected_response);

    server.stop();
}
