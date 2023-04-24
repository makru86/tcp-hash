#include <boost/asio.hpp>
#include <boost/process/child.hpp>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <filesystem>
#include <libtcp_hash/Server.h>
#include <thread>

using namespace std::chrono_literals;
using namespace boost::asio;
namespace libtcp_hash {

BOOST_AUTO_TEST_SUITE(ServerTests)

BOOST_AUTO_TEST_CASE(ClientTest) {
  LOG_DEBUG(std::filesystem::current_path().c_str());
  io_context ioContext;

  boost::process::child server{"../../prototype/tcp_hash.sh 4321"};
  LOG_DEBUG("server started on port 4321");

  //  boost::process::async_system(
  //      ioContext,
  //      [](int exit_code, const std::error_code &ec) {
  //        std::cout << "hello world, I exited with " << exit_code <<
  //        std::endl;
  //      },
  //      "../../prototype/tcp_hash.sh");

  std::this_thread::sleep_for(5s);
  server.terminate();
  server.wait();
  LOG_DEBUG("server terminated with code " << server.exit_code());
}

BOOST_AUTO_TEST_CASE(ServerTest) {
  io_service io_service;
  SimpleTcpListener server(io_service, ip::tcp::endpoint(ip::tcp::v4(), 1234));

  signal_set signals(io_service, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io_service.stop(); });

  LOG_DEBUG("Running...")
  std::thread server_thread{[&]() { io_service.run_for(1s); }};

  //  for (;;) {
  //    std::cout << "Commands:\n q: stop server\n> ";
  //    if (std::getchar() == 'q') {
  //      io_service.stop();
  //      break;
  //    }
  //  }
  //  io_service.stop();
  server_thread.join();
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