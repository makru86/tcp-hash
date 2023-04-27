#pragma once
namespace libtcp_hash {
using Message = std::string_view;
using Byte = Message::value_type;

template <typename Handler>
Session<Handler>::Session(boost::asio::ip::tcp::socket socket, Handler handler)
    : socket_(std::move(socket)), handler_{handler} {
  LOG_DEBUG(this << ": new connection: " << socket_.remote_endpoint());
}

template <typename Handler> Session<Handler>::~Session() {
  LOG_DEBUG(this << ": close connection: " << socket_.remote_endpoint());
}

template <typename Handler> void Session<Handler>::receiveMessage() {
  auto self(this->shared_from_this());
  LOG_DEBUG("receiving...");
  socket_.async_read_some(
      boost::asio::buffer(buffer_, bufferSize_),
      [this, self](boost::system::error_code ec, std::size_t length) {
        LOG_DEBUG("received: " << length << "ec: " << ec);
        if (!ec) {
          handler_.onMessageReceived(Message(buffer_, length),
                                     [](Message messageToSend) {

                                     });
          receiveMessage();
        }
      });
}

template <typename Handler>
void Session<Handler>::sendMessage(Message message) {
  auto self(this->shared_from_this());
  LOG_DEBUG("sending: " << message.size() << " bytes");
  async_write(socket_, boost::asio::buffer(message.data(), message.size()),
              [this, self](boost::system::error_code ec, std::size_t length) {
                LOG_DEBUG("sent: " << length << "ec: " << ec);
                if (!ec) {
                  receiveMessage();
                }
              });
}

} // namespace libtcp_hash