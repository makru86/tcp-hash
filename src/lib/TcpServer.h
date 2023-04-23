#pragma once

#include "Log.h"
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace app {

using boost::asio::ip::tcp;

class TcpServer {

  boost::asio::io_context &io_context_;
  tcp::acceptor acceptor_;
  std::vector<std::thread> workers_;

public:
  TcpServer(boost::asio::io_context &io_context, const tcp::endpoint &endpoint,
            std::size_t num_threads)
      : io_context_(io_context), acceptor_(io_context, endpoint) {
    LOG_DEBUG("Listen TCP endpoint: " << endpoint);
    LOG_DEBUG("Number of threads: " << num_threads);
    for (std::size_t i = 0; i < num_threads; ++i) {
      workers_.emplace_back([this]() { io_context_.run(); });
    }
  }

  void start() { do_accept(); }

  void stop() {
    io_context_.stop();

    for (auto &worker : workers_) {
      worker.join();
    }
  }

  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            LOG_DEBUG("Connection from " << socket.remote_endpoint());

            //                    socket.set_option(tcp::no_delay(true));
            //                    socket.set_option(boost::asio::socket_base::send_buffer_size(65536));
            //                    socket.set_option(boost::asio::socket_base::receive_buffer_size(65536));

            // Handle connection in thread pool
            io_context_.post([&/*socket = std::move(socket)*/]() mutable {
              handle_connection(std::move(socket));
            });
          }

          do_accept();
        });
  }

  static void handle_connection(tcp::socket socket) try {
    // Read data from client
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    std::string data = boost::asio::buffer_cast<const char *>(buf.data());
    LOG_DEBUG("Received: " << data);

    // Send response to client
    std::string response = "Hello, " + data;
    boost::asio::write(socket, boost::asio::buffer(response));

    LOG_DEBUG("Response sent.");
  } catch (std::exception &e) {
    LOG_ERROR("Exception in thread: " << e.what());
  }
};

} // namespace app