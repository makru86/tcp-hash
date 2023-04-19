#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace app {

    class TcpServer {
    public:

        using tcp = boost::asio::ip::tcp;

        TcpServer(boost::asio::io_context &io_context, const tcp::endpoint &endpoint, std::size_t num_threads)
                : io_context_(io_context), acceptor_(io_context, endpoint) {
            // Create worker threads
            for (std::size_t i = 0; i < num_threads; ++i) {
                workers_.emplace_back([this]() {
                    io_context_.run();
                });
            }
        }

        void start() {
            do_accept();
        }

        void stop() {
            io_context_.stop();

            for (auto &worker: workers_) {
                worker.join();
            }
        }

    private:
        void do_accept() {
            acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "Connection from " << socket.remote_endpoint() << std::endl;

                    // Handle connection in thread pool
                    io_context_.post([&]() mutable {
                        handle_connection(std::move(socket));
                    });
                }

                do_accept();
            });
        }

        static void handle_connection(tcp::socket socket) {
            try {
                // Read data from client
                boost::asio::streambuf buf;
                boost::asio::read_until(socket, buf, "\n");
                std::string data = boost::asio::buffer_cast<const char *>(buf.data());
                std::cout << "Received: " << data << std::endl;

                // Send response to client
                std::string response = "Hello, " + data;
                boost::asio::write(socket, boost::asio::buffer(response));

                std::cout << "Response sent." << std::endl;
            }
            catch (std::exception &e) {
                std::cerr << "Exception in thread: " << e.what() << std::endl;
            }
        }

        boost::asio::io_context &io_context_;
        tcp::acceptor acceptor_;
        std::vector<std::thread> workers_;
    };
}