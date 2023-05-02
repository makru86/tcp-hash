#include <algorithm>
#include <boost/asio.hpp>
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

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket, size_t session_id) : socket_(std::move(socket)) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            LOG_DEBUG("NN\n,received:'" << std::string_view(data_, length)
                                        << "'," << length);

            libtcp_hash::tokenizer(
                StrView(data_, length), //
                [&](StrView token, bool final) {
                  LOG_DEBUG("NN\nhashing:'" << token << "'," << token.size()
                                            << "," << final);
                  fsm.feed(token);
                  if (final) {
                    HashValue hash = fsm.digest();
                    std::string str = hexen(hash) + '\n';
                    assert(str.size() < max_length);
                    std::copy(str.cbegin(), str.cend(), data_out_);
                    do_write(str.size());
                  }
                });
            do_read();
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    LOG_DEBUG("NN\nsending:'" << std::string_view(data_out_, length) << "'");
    asio::async_write(socket_, asio::buffer(data_out_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                          //                          do_read();
                        }
                      });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char data_out_[max_length];
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
        std::make_shared<session>(std::move(socket_), next_session_id_)
            ->start();
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  size_t next_session_id_{0};
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
