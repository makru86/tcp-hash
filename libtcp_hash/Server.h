/*
 * Session and SimpleTcpListener are inspired by Boost Asio example
 * async_tcp_echo_server.cpp:
 * https://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
*/

#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <libtcp_hash/Log.h>
#include <string>
#include <thread>
#include <vector>

namespace libtcp_hash {

using namespace boost::asio;

class Session : public std::enable_shared_from_this<Session> {
  ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];

public:
  Session(ip::tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    async_write(
        socket_, buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            do_read();
          }
        });
  }
};

// LOG_DEBUG("listening TCPv4 address: " << endpoint);
// LOG_DEBUG("number of threads: " << num_threads);
// LOG_DEBUG("new TCP connection from: " << socket.remote_endpoint());
// LOG_DEBUG("bytes received: " << data);
// LOG_DEBUG("response sent");
// LOG_ERROR("Exception in thread: " << e.what());
//
//                     socket.set_option(ip::tcp::no_delay(true));
//                     socket.set_option(socket_base::send_buffer_size(65536));
//                     socket.set_option(socket_base::receive_buffer_size(65536));

class SimpleTcpListener {
  ip::tcp::acceptor acceptor_;
  ip::tcp::socket socket_;

public:
  SimpleTcpListener(io_context &io_context, const ip::tcp::endpoint &endpoint)
      : acceptor_(io_context, endpoint), socket_(io_context) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket_))->start();
      }

      do_accept();
    });
  }
};

/*
 * TcpHashClient:
 *  keep-alive - don't close connection after request
 *  pipelining - don't wait response before sending next request
 */
class TcpHashClient {
  io_context io_context_;
  ip::tcp::resolver resolver_;
  ip::tcp::socket socket_;

public:
  TcpHashClient(std::string_view address, std::string_view port)
      : resolver_(io_context_), socket_(io_context_) {
    connect(socket_, resolver_.resolve(address, port));
  }

  [[nodiscard]] bool connected() const { return socket_.is_open(); }

  std::string request(const std::string &request) {
    write(socket_, buffer(request));

    streambuf response;
    read_until(socket_, response, "\n");

    std::istream response_stream(&response);
    std::string response_str;
    std::getline(response_stream, response_str);
    return response_str;
  }
};

} // namespace libtcp_hash