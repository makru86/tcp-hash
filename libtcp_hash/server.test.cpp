#include <boost/asio.hpp>
#include <boost/process/child.hpp>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <filesystem>
#include <libtcp_hash/server.hpp>
#include <thread>

using namespace std::chrono_literals;
using namespace boost::asio;
namespace libtcp_hash {

BOOST_AUTO_TEST_SUITE(ServerTests, TEST_TIMEOUT_SECS(16))

BOOST_AUTO_TEST_CASE(ServerClientTest) {
  io_service io;
  ip::tcp::endpoint tcpAddress{ip::tcp::v4(), 1234};
  EchoProtocol echoProtocol;
  SimpleTcpListener server{io, tcpAddress, echoProtocol};
  TcpHashClient client{io, tcpAddress};
  std::thread ioThread{[&]() { io.run(); }};
  BOOST_TEST("echo Hello" == client.request("echo Hello\n"));

  io.stop();
  ioThread.join();
  LOG_DEBUG("server terminated");
}

//    io_context io_context;
//    tcp::endpoint endpoint(tcp::v4(), 1234);
//
//    TcpServer server(io_context, endpoint,
//    std::thread::hardware_concurrency());
//
//    tcp::socket socket(io_context);
//    socket.connect(endpoint);
//
//    std::string message = "World\n";
//    write(socket, buffer(message));
//
//    io_context.run();
//
//    std::string expected_response = "Hello, World\n";
//    streambuf response_buf;
//    read_until(socket, response_buf, "\n");
//    std::string actual_response = buffer_cast<const
//    char*>(response_buf.data());
//
//    BOOST_CHECK_EQUAL(actual_response, expected_response);
//
//    server.stop();

BOOST_AUTO_TEST_SUITE_END()

} // namespace libtcp_hash