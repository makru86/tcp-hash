#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <memory>
#include <string>
#include <utility>

using namespace libtcp_hash;

namespace asio = boost::asio;
using asio::ip::tcp;

using FSM = libtcp_hash::StatefulHasher<XxHash>;
libtcp_hash::XxHash xxHash;
FSM fsm{xxHash};
asio::static_thread_pool static_thread_pool{1};

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket,
          std::weak_ptr<asio::static_thread_pool> thread_pool)
      : socket_(std::move(socket)), thread_pool_{thread_pool} {}

  void start() { do_read(); }

  void write(std::string data) {
    assert(data.size() < max_length);
    std::copy(data.cbegin(), data.cend(), write_data_);
    do_write(data.size());
  }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            if (auto thread_pool = thread_pool_.lock()){
              asio::post(*thread_pool, [thread_pool](){
                process(std::string{data_, length},
                  нельля здесь this - другой executor
                        std::bind(&session::write, this, std::placeholders::_1));
              });
            }
            do_read();
          }
        });
  }

  // process
  template <typename OnResultCb> void process(std::string data, OnResultCb cb) {
    auto length = data.size();
    LOG_DEBUG("NN\n,received:'" << data << "'," << length);
    libtcp_hash::tokenizer(data, // on token:
                           [&](StrView token, bool final) {
                             LOG_DEBUG("NN\nhashing:'" << token << "',"
                                                       << token.size() << ","
                                                       << final);
                             fsm.feed(token);
                             if (final) {
                               HashValue hash = fsm.digest();
                               std::string hash_str = to_hex_str(hash) + '\n';
                               cb(std::move(hash_str));
                             }
                           });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    LOG_DEBUG("NN\nsending:'" << std::string_view(write_data_, length) << "'");
    asio::async_write(socket_, asio::buffer(write_data_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                          //                          do_read();
                        }
                      });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char write_data_[max_length];
  std::weak_ptr<asio::static_thread_pool> thread_pool_;
};

class server {
public:
  server(asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
      if (!ec) {
        std::make_shared<session>(
            std::move(socket_),
            std::weak_ptr<asio::static_thread_pool>{thread_pool_})
            ->start();
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  size_t next_session_id_{0};
  std::shared_ptr<asio::static_thread_pool> thread_pool_{
      std::make_shared<asio::static_thread_pool>(1)};
};

BOOST_AUTO_TEST_SUITE(fffs)

BOOST_AUTO_TEST_CASE(TokenizerTest) {
  try {

    asio::io_context io_context;

    server s(io_context, 1234);

    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}

BOOST_AUTO_TEST_SUITE_END()
