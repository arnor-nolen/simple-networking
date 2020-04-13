#include <boost/asio.hpp>
#include <iostream>
#include <set>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

struct chat_participant {
  virtual ~chat_participant() {}
  virtual void send(const std::string &) = 0;
};

struct chat_room {
  chat_room() {}

  void join(const std::shared_ptr<chat_participant> &cp) {
    participants_.insert(cp);
    messages_.emplace_back("New user connected, total of " +
                           std::to_string(participants_.size()) + "!\n");
    for (const auto &i : messages_) {
      cp->send(i);
    }
  }

  void leave(const std::shared_ptr<chat_participant> &cp) {
    participants_.erase(cp);
  }

  void send(const std::string &message) {
    std::cout << participants_.size() << '\n';
    for (auto &c : participants_) {
      c->send(message);
    }
  }

private:
  std::set<std::shared_ptr<chat_participant>> participants_;
  std::vector<std::string> messages_;
};

struct connection : public chat_participant,
                    public std::enable_shared_from_this<connection> {
  static std::shared_ptr<connection> create(asio::io_context &ioc,
                                            chat_room &room) {
    return std::shared_ptr<connection>(new connection(ioc, room));
  }

  tcp::socket &socket() { return socket_; }

  void read() {
    // Read for connection end
    auto self = shared_from_this();
    std::array<char, 128> buf{""};
    asio::async_read(socket_, asio::buffer(buf),
                     [this, self](const error_code &error, const size_t &size) {
                       connection::handle_read(error, size);
                     });
  }

  void send(const std::string &message) {
    auto self = shared_from_this();
    asio::async_write(
        socket_, asio::buffer(message),
        [this, self](const error_code &error, const size_t &size) {
          connection::handle_read(error, size);
        });
  }

private:
  connection(asio::io_context &ioc, chat_room &room)
      : socket_(ioc), chat_room_(room) {}

  void handle_read(const error_code &error, const size_t &) {
    if (!error)
      std::cout << "Message read!\n";
    else if (error == asio::error::connection_reset ||
             error == asio::error::eof)
      chat_room_.leave(shared_from_this());
    else
      std::cerr << "Error: " << error.message() << "\n";
  }

  void handle_write(const error_code &error, const size_t &) {
    if (!error)
      std::cout << "Message sent!\n";
    else
      std::cerr << "Error: " << error.message() << "\n";
  }

  tcp::socket socket_;
  chat_room &chat_room_;
};

struct server {
  server(asio::io_context &ioc, const tcp::endpoint &endpoint,
         const chat_room &chat_room)
      : ioc_(ioc), acceptor_(ioc, endpoint), chat_room_(chat_room) {
    start_accept();
  }

  void start_accept() {
    auto new_connection = connection::create(ioc_, chat_room_);
    acceptor_.async_accept(new_connection->socket(),
                           [this, new_connection](const error_code &error) {
                             server::handle_accept(new_connection, error);
                           });
  }

  void handle_accept(const std::shared_ptr<connection> &new_connection,
                     const error_code &error) {
    if (!error) {
      std::cout << "Connection accepted!\n";
      chat_room_.join(new_connection);
      new_connection->read();
    } else {
      std::cerr << "Error: " << error.message() << "\n";
    }
    start_accept();
  }

private:
  asio::io_context &ioc_;
  tcp::acceptor acceptor_;
  chat_room chat_room_;
};

int main(int argc, char *argv[]) {
  try {
    const short port = 13;

    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), port);

    chat_room cr;
    server s(ioc, endpoint, cr);

    ioc.run();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}