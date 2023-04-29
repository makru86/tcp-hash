/*
 * Session and SimpleTcpListener are inspired by Boost Asio example
 * async_tcp_echo_server.cpp:
 * https://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
 */

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <chrono>
#include <iostream>
#include <libtcp_hash/util.hpp>
#include <string>
#include <thread>
#include <vector>

namespace libtcp_hash {

// template <typename SendMessageCb>
struct EchoProtocol {
  using SendMessageCb = std::function<void(std::string_view)>;
  static void onMessageReceived(std::string_view message,
                         SendMessageCb sendMessageCb) {
    sendMessageCb(message);
  }
};

template <typename Protocol>
class Session : public std::enable_shared_from_this<Session<Protocol>> {

  /*
  % cat /proc/sys/net/ipv4/tcp_wmem
  4096	16384	4194304
  % cat /proc/sys/net/ipv4/tcp_rmem
  4096	131072	6291456
   */
  static constexpr size_t bufferSize_{8192};
  char buffer_[bufferSize_];
  boost::asio::ip::tcp::socket socket_;
  Protocol handler_;

public:
  Session(boost::asio::ip::tcp::socket socket, Protocol handler);
  ~Session();

  void start() { receiveMessage(); }
  void receiveMessage();
  void sendMessage(std::string_view message);
};

// LOG_DEBUG("number of threads: " << num_threads);
// LOG_DEBUG("bytes received: " << data);
// LOG_DEBUG("response sent");
// LOG_ERROR("Exception in thread: " << e.what());
//
//                     socket.set_option(ip::tcp::no_delay(true));
//                     socket.set_option(socket_base::send_buffer_size(65536));
//                     socket.set_option(socket_base::receive_buffer_size(65536));

template <typename Handler> class SimpleTcpListener {

  boost::asio::io_service &io_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
  Handler handler_;

public:
  SimpleTcpListener(boost::asio::io_context &io,
                    const boost::asio::ip::tcp::endpoint &endpoint,
                    Handler handler)
      : io_{io}, acceptor_(io_, endpoint), socket_(io_), handler_{handler} {
    LOG_DEBUG("listening: " << endpoint);
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
      if (!ec) {
        std::make_shared<Session<Handler>>(std::move(socket_), handler_)
            ->start();
      }

      do_accept();
    });
  }
};

} // namespace libtcp_hash

#include <libtcp_hash/server.tpp>