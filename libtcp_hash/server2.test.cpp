#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/test/unit_test.hpp>
#include <compare>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <memory>
#include <string>
#include <thread>
#include <utility>

using namespace std::chrono_literals;
using namespace libtcp_hash;

namespace asio = boost::asio;
using asio::ip::tcp;

using FSM = libtcp_hash::StatefulHasher<XxHash>;
libtcp_hash::XxHash xxHash;
FSM fsm{xxHash};

template <typename OnHashCb> void process(std::string data, OnHashCb on_hash) {
  LOG_DEBUG("NN\n,received:" << data.size());
  LOG_DEBUG("NN\n,sleeping...");
    std::this_thread::sleep_for(10s);
  LOG_DEBUG("NN\n,woke up");
  auto &&on_token = [&](StrView token, bool final) {
    LOG_DEBUG("NN\nhashing:" << token.size() << "," << final);
    fsm.feed(token);
    if (final) {
      HashValue hash = fsm.digest();
      std::string hash_str = to_hex_str(hash) + '\n';
      on_hash(std::move(hash_str));
    }
  };
  libtcp_hash::tokenizer(data, on_token);
}

class session : public std::enable_shared_from_this<session> {

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char write_data_[max_length];
  asio::static_thread_pool &thread_pool_;

public:
  session(tcp::socket socket, asio::static_thread_pool &thread_pool)
      : socket_(std::move(socket)), thread_pool_{thread_pool} {}

  void start() { do_read(); }

  bool is_open() const { return socket_.is_open(); }

  void stop() { socket_.close(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            auto &&data{std::string{data_, length}};
            async_process(data, thread_pool_, self);
            do_read();
          }
        });
  }

  static void async_process(std::string data,
                            asio::static_thread_pool &thread_pool,
                            std::shared_ptr<session> ses) {

    asio::post(
        thread_pool, [ses = std::move(ses), data = std::move(data)]() mutable {
          if (ses->is_open()) {
            LOG_DEBUG("NN\nsession open");
            auto weak{std::weak_ptr<session>{ses}};
            process(std::move(data),
                    std::bind(&session::on_hash, weak, std::placeholders::_1));
          } else {
            LOG_DEBUG("NN\nsession closed");
          }
        });
  }

  static void on_hash(std::weak_ptr<session> weak_self, std::string hash) {
    auto self{weak_self.lock()};
    if (self) {
      self->write(hash);
    }
  }

  void write(std::string data) {
    assert(data.size() < max_length);
    std::copy(data.cbegin(), data.cend(), write_data_);
    do_write(data.size());
  }

  void do_write(std::size_t length) {
    if (!is_open()) {
      LOG_DEBUG("NN\nsession closed");
      stop();
      return;
    }
    auto self(shared_from_this());
    LOG_DEBUG("NN\nsending:" << length);
    asio::async_write(socket_, asio::buffer(write_data_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (ec) {
                          self->stop();
                        }
                      });
  }
};

class server {
  struct single_thread_pool : asio::static_thread_pool {
    single_thread_pool() : asio::static_thread_pool{1} {}
  };

  constexpr static auto thread_count_{3};
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  std::array<single_thread_pool, thread_count_> threads_{};
  size_t session_number_{0};
  asio::signal_set signals_;

public:
  server(asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context), signals_{io_context, SIGINT, SIGTERM} {
    signals_.async_wait([&](auto, auto) { io_context.stop(); });
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
      if (!ec) {
        // Round robin algorigthm selecting thread for work.
        auto index{session_number_++ % thread_count_};
        std::make_shared<session>(std::move(socket_), threads_[index])->start();
      }
      do_accept();
    });
  }
};

BOOST_AUTO_TEST_SUITE(fffs)

BOOST_AUTO_TEST_CASE(TokenizerTest) {
  try {

    LOG_DEBUG("main");
    asio::io_context io_context;

    server server(io_context, 1234);

    //    std::thread t{[&](){
    //      io_context.run();
    //    }};
    io_context.run();
    //    t.join();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}

BOOST_AUTO_TEST_SUITE_END()
