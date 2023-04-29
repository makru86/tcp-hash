//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include "boost/asio.hpp"
#include "handler_allocator.hpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <boost/mem_fn.hpp>
#include <iostream>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <list>
#include <string>

namespace asio = boost::asio;
namespace libtcp_hash {

/* LoadtestConfig:
 * Loadtest configuration parameters.
 */
struct LoadtestConfig {
  int clientSessions{getEnvOrValue("LOADTEST_CLIENT_SESSIONS", 8)};
  int testingTime_{getEnvOrValue("LOADTEST_SECONDS", 10)};
  size_t dataSize{10 * 1024 * 1024};
  int repeatDataIterations{100};
  unsigned randomGeneratorSeed{123};
  boost::asio::ip::tcp::endpoint tcpAddress{boost::asio::ip::tcp::v4(), 1234};
};

struct LoadtestMetrics {
  std::atomic<uint64_t> timestampStart{};
  std::atomic<uint64_t> timestampStop{};
  std::atomic<uint64_t> bytesSent{};
  std::atomic<uint64_t> bytesReceived{};
  std::atomic<uint64_t> messagesSent{};
  std::atomic<uint64_t> messagesReceived{};

  void add(size_t bytes_written, size_t bytes_read) {
    LOG_DEBUG("addu");
    asio::detail::mutex::scoped_lock lock(mutex_);
    bytesSent += bytes_written;
    bytesReceived += bytes_read;
  }

  void addMessage() {
    LOG_DEBUG("addMessage");
    asio::detail::mutex::scoped_lock lock(mutex_);
    messagesReceived += 1;
  }

  std::string makeReport() {
    double seconds{};
    double megaHashesPerSecond{};
    double avgDataSize{};

    {
      asio::detail::mutex::scoped_lock lock(mutex_);
      seconds = (timestampStop - timestampStart) / 1e9;
      avgDataSize = 1.0 * bytesSent.load() / messagesReceived.load();
      megaHashesPerSecond = messagesReceived.load() / 1e6 / seconds;
      // avgDataSize:
      //    For a uniformly-distributed char random number generator that
      //    produces values in the range [0, 255], the average input line size
      //    in the limit tends to 255 B.
    }

    std::stringstream report;
    report << ""                                                   //
           << "\n Hashrate:"                                       //
           << "\n   " << megaHashesPerSecond << " MH/s"            //
           << "\n Latency:"                                        //
           << "\n   TODO"                                          //
           << "\n Average size: " << avgDataSize << " B"           //
           << "\n Total time:   " << seconds << " s"               //
           << "\n Messages:"                                       //
           << "\n   sent:       " << messagesSent                  //
           << "\n   received:   " << messagesReceived              //
           << "\n Bytes:"                                          //
           << "\n   sent:       " << bytesSent / 1e6 << " MiB"     //
           << "\n   received:   " << bytesReceived / 1e6 << " MiB" //
           << "\n";
    return report.str();
  }

private:
  asio::detail::mutex mutex_;
};

std::ostream &operator<<(std::ostream &os, LoadtestConfig const &value);
std::ostream &operator<<(std::ostream &os, LoadtestMetrics const &value);

class session {
  asio::strand<asio::io_context::executor_type> strand_;
  asio::ip::tcp::socket socket_;
  size_t block_size_;
  char *read_data_;
  size_t read_data_length_;
  char *write_data_;
  int unwritten_count_;
  size_t bytes_written_;
  size_t bytes_read_;
  LoadtestMetrics &stats_;
  HandlerAllocator read_allocator_;
  HandlerAllocator write_allocator_;

public:
  session(asio::io_context &ioc, size_t block_size, LoadtestMetrics &stats)
      : strand_(ioc.get_executor()), socket_(ioc), block_size_(block_size),
        read_data_(new char[block_size]), read_data_length_(0),
        write_data_(new char[block_size]), unwritten_count_(0),
        bytes_written_(0), bytes_read_(0), stats_(stats) {
    for (size_t i = 0; i < block_size_; ++i)
      write_data_[i] = static_cast<char>(i % 128);
  }

  ~session() {
    stats_.add(bytes_written_, bytes_read_);

    delete[] read_data_;
    delete[] write_data_;
  }

  void start(const asio::ip::tcp::resolver::results_type &endpoints) {
    asio::async_connect(
        socket_, endpoints,
        asio::bind_executor(strand_, boost::bind(&session::handle_connect, this,
                                                 asio::placeholders::error)));
  }

  void stop() {
    asio::post(strand_, boost::bind(&session::close_socket, this));
  }

private:
  void handle_connect(const boost::system::error_code &err) {
    if (err) {
      LOG_ERROR(err);
      return;
    }
    LOG_DEBUG("connected");
    boost::system::error_code set_option_err;
    asio::ip::tcp::no_delay no_delay(true);
    socket_.set_option(no_delay, set_option_err);
    if (!set_option_err) {
      ++unwritten_count_;
      async_write(socket_, asio::buffer(write_data_, block_size_),
                  asio::bind_executor(
                      strand_,
                      make_custom_alloc_handler(
                          write_allocator_,
                          boost::bind(&session::handle_write, this,
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred))));
      socket_.async_read_some(
          asio::buffer(read_data_, block_size_),
          asio::bind_executor(
              strand_,
              make_custom_alloc_handler(
                  read_allocator_,
                  boost::bind(&session::handle_read, this,
                              asio::placeholders::error,
                              asio::placeholders::bytes_transferred))));
    }
  }

  void handle_read(const boost::system::error_code &err, size_t length) {
    if (err) {
      LOG_ERROR(err);
      return;
    }
    LOG_DEBUG("handle_read:" << length);
    bytes_read_ += length;
    read_data_length_ = length;
    ++unwritten_count_;
    if (unwritten_count_ == 1) {
      tokenizer(std::string_view(read_data_, length),
                [this](std::string_view chunk, bool separatorFound) {
                  if (separatorFound) {
                    stats_.addMessage();
                  }
                });

      //       for (auto it{0}; it != read_data_length_; ++it) {
      //         if (read_data_[it] == '\n') {
      //           stats_.addMessage();
      //         }
      //       }
      //
      std::swap(read_data_, write_data_);
      async_write(socket_, asio::buffer(write_data_, read_data_length_),
                  asio::bind_executor(
                      strand_,
                      make_custom_alloc_handler(
                          write_allocator_,
                          boost::bind(&session::handle_write, this,
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred))));
      socket_.async_read_some(
          asio::buffer(read_data_, block_size_),
          asio::bind_executor(
              strand_,
              make_custom_alloc_handler(
                  read_allocator_,
                  boost::bind(&session::handle_read, this,
                              asio::placeholders::error,
                              asio::placeholders::bytes_transferred))));
    }
  }

  void handle_write(const boost::system::error_code &err, size_t length) {
    if (err) {
      LOG_ERROR(err);
      return;
    }
    LOG_DEBUG("handle_write:" << length);
    bytes_written_ += length;

    --unwritten_count_;
    if (unwritten_count_ == 1) {
      std::swap(read_data_, write_data_);
      async_write(socket_, asio::buffer(write_data_, read_data_length_),
                  asio::bind_executor(
                      strand_,
                      make_custom_alloc_handler(
                          write_allocator_,
                          boost::bind(&session::handle_write, this,
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred))));
      socket_.async_read_some(
          asio::buffer(read_data_, block_size_),
          asio::bind_executor(
              strand_,
              make_custom_alloc_handler(
                  read_allocator_,
                  boost::bind(&session::handle_read, this,
                              asio::placeholders::error,
                              asio::placeholders::bytes_transferred))));
    }
  }

  void close_socket() {
    LOG_DEBUG("close socket");
    socket_.close();
  }
};

/*
 * TcpHashClient:
 *  keep-alive - don't close connection after request
 *  pipelining - don't wait response before sending next request
 *
 * Client taken from Asio tests.
 * https://github.com/chriskohlhoff/asio/blob/master/asio/src/tests/performance/client.cpp
 */
class TcpHashClient {
  asio::io_context &io_context_;
  asio::steady_timer stop_timer_;
  std::list<session *> sessions_;
  LoadtestMetrics &stats_;

public:
  struct ErrorResolvingName : std::runtime_error {
    ErrorResolvingName() : std::runtime_error{"ErrorResolvingName"} {}
  };

  TcpHashClient(asio::io_context &ioc, const asio::ip::tcp::endpoint &endpoint,
                size_t block_size, size_t session_count, int timeout,
                LoadtestMetrics &stats) noexcept(false)
      : io_context_(ioc), stop_timer_(ioc), stats_{stats} //
  {
    stop_timer_.expires_after(asio::chrono::seconds(timeout));
    stop_timer_.async_wait(boost::bind(&TcpHashClient::handle_timeout, this));

    auto resolver{asio::ip::tcp::resolver{io_context_}};
    auto endpoints{resolver.resolve(endpoint)};
    if (endpoints.empty()) {
      throw ErrorResolvingName{};
    }

    for (size_t i = 0; i < session_count; ++i) {
      session *new_session = new session(io_context_, block_size, stats_);
      new_session->start(endpoints);
      sessions_.push_back(new_session);
    }
  }

  ~TcpHashClient() {
    while (!sessions_.empty()) {
      delete sessions_.front();
      sessions_.pop_front();
    }
  }

  void handle_timeout() {
    std::for_each(sessions_.begin(), sessions_.end(),
                  boost::mem_fn(&session::stop));
  }
};

inline int master(int argc, char *argv[]) {
  try {
    if (argc != 7) {
      std::cerr << "Usage: TcpHashClient <host> <port> <threads> <blocksize> ";
      std::cerr << "<sessions> <time>\n";
      return 1;
    }

    using namespace std; // For atoi.
    const char *host = argv[1];
    const char *port = argv[2];
    int thread_count = atoi(argv[3]);
    size_t block_size = atoi(argv[4]);
    size_t session_count = atoi(argv[5]);
    int timeout = atoi(argv[6]);

    asio::io_context ioc;

    asio::ip::tcp::resolver r(ioc);
    asio::ip::tcp::resolver::results_type endpoints = r.resolve(host, port);

    LoadtestMetrics stats{};
    TcpHashClient client(ioc, *endpoints, block_size, session_count, timeout,
                         stats);

    std::list<std::thread *> threads;
    while (--thread_count > 0) {
      std::thread *new_thread =
          new std::thread(boost::bind(&asio::io_context::run, &ioc));
      threads.push_back(new_thread);
    }

    ioc.run();

    while (!threads.empty()) {
      threads.front()->join();
      delete threads.front();
      threads.pop_front();
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

} // namespace libtcp_hash