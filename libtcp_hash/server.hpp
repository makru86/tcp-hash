#pragma once
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/thread_pool.hpp>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>

namespace libtcp_hash {

using namespace std::chrono_literals;
using namespace libtcp_hash;

namespace asio = boost::asio;
using asio::ip::tcp;

template <class HasherType>
class Session : public std::enable_shared_from_this<Session<HasherType>> {

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char write_data_[max_length];
  asio::thread_pool &thread_pool_;
  std::shared_ptr<HasherType> hasher_;

public:
  using Self = Session<HasherType>;
  using Ptr = std::shared_ptr<Self>;

private:
  Session(tcp::socket socket, asio::thread_pool &thread_pool,
          std::shared_ptr<HasherType> hasher)
      : socket_(std::move(socket)), thread_pool_{thread_pool}, hasher_{hasher} {
  }

public:
  [[nodiscard]] static Ptr create(tcp::socket socket,
                                  asio::thread_pool &thread_pool,
                                  std::shared_ptr<HasherType> hasher) {
    // Not using std::make_shared<> because the c'tor is private.
    return Ptr{new Self{std::move(socket), thread_pool, std::move(hasher)}};
  }

  void start() { do_read(); }

  bool is_open() const { return socket_.is_open(); }

  void stop() { socket_.close(); }

private:
  Ptr get_shared_from_this() { return this->shared_from_this(); };

  void do_read() {
    auto self(get_shared_from_this());
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

  static void async_process(std::string data, asio::thread_pool &thread_pool,
                            std::shared_ptr<Session> ses) {

    asio::post(
        thread_pool, [ses = std::move(ses), data = std::move(data)]() mutable {
          if (ses->is_open()) {
            auto session_weak_ptr{std::weak_ptr<Session>{ses}};

            ses->hasher_->process(std::move(data),
                                  std::bind(&Session::on_hash, session_weak_ptr,
                                            std::placeholders::_1));
          } else {
            LOG_DEBUG("session closed");
          }
        });
  }

  static void on_hash(std::weak_ptr<Session> weak_self, std::string hash) {
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
      LOG_DEBUG("session closed");
      stop();
      return;
    }
    auto self(get_shared_from_this());
    LOG_DEBUG("sending:" << length);
    asio::async_write(socket_, asio::buffer(write_data_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (ec) {
                          self->stop();
                        }
                      });
  }
};

template <class Hasher> class Server {
  struct single_thread_pool : asio::thread_pool {
    single_thread_pool() : asio::thread_pool{1} {}
  };

public:
  constexpr static auto thread_count_{3};

private:
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  std::array<single_thread_pool, thread_count_> threads_{};
  size_t session_number_{0};
  asio::signal_set signals_;
  asio::steady_timer dump_metrics_timer_;
  std::shared_ptr<Hasher> hasher_;

public:
  Server(asio::io_context &io_context, short port,
         std::shared_ptr<Hasher> hasher)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context), signals_{io_context, SIGINT, SIGTERM},
        dump_metrics_timer_(io_context), hasher_{hasher} {
    signals_.async_wait([&](auto, auto) { io_context.stop(); });
    dump_metrics();
    do_accept();
  }

  void stop() {
    boost::system::error_code ec;
    acceptor_.cancel(ec);
    LOG_DEBUG("stop:cancel=" << ec);
    acceptor_.close(ec);
    LOG_DEBUG("stop:close=" << ec);
    for (auto &thread_pool : threads_) {
      thread_pool.stop();
    }
  }

private:
  constexpr static auto dump_metrics_delay_{5s};

  void dump_metrics() {
    LOG_DEBUG("Dumping metrics");
    dump_metrics_timer_.expires_after(dump_metrics_delay_);
    dump_metrics_timer_.async_wait([this](std::error_code ec) {
      if (ec) {
        return;
      }
      dump_metrics();
    });
  }

  void do_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
      if (!ec) {
        // Round robin algorigthm selecting thread for work.
        auto index{session_number_++ % thread_count_};

        Session<Hasher>::create(std::move(socket_), threads_[index],
                                hasher_->create())
            ->start();
      }
      do_accept();
    });
  }
};

} // namespace libtcp_hash
