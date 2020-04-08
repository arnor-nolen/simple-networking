#include <boost/asio.hpp>
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

struct connection : public std::enable_shared_from_this<connection> {
  static std::shared_ptr<connection> create(asio::io_context &ioc,
                                            const std::string &message) {
    return std::shared_ptr<connection>(new connection(ioc, message));
  }
  tcp::socket &socket() { return socket_; }
  void start() {
    error_code error;
    asio::async_write(socket_, asio::buffer(message_),
                      std::bind(&connection::handle_write, shared_from_this(),
                                std::placeholders::_1, std::placeholders::_2));
  }

private:
  connection(asio::io_context &ioc, const std::string &message)
      : socket_(ioc), message_(message) {}
  void handle_write(const error_code &error, const size_t &) {
    if (!error)
      std::cout << "Message sent!\n";
    else
      std::cerr << "Error: " << error.message() << "\n";
  }

  tcp::socket socket_;
  std::string message_;
};

struct server {
  server(asio::io_context &ioc, const tcp::endpoint &endpoint,
         const std::string &message)
      : ioc_(ioc), acceptor_(ioc, endpoint), message_(message) {
    start_accept();
  }

  void start_accept() {
    auto new_connection = connection::create(ioc_, message_);
    acceptor_.async_accept(new_connection->socket(),
                           std::bind(&server::handle_accept, this,
                                     new_connection, std::placeholders::_1));
  }

  void handle_accept(std::shared_ptr<connection> &new_connection,
                     const error_code &error) {
    if (!error) {
      std::cout << "Connection accepted!\n";
      new_connection->start();
    }
    start_accept();
  }

private:
  asio::io_context &ioc_;
  tcp::acceptor acceptor_;
  std::string message_;
};

int main(int argc, char *argv[]) {
  try {
    const short port = 13;
    const std::string message = "Hello, world!";

    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), port);

    server s(ioc, endpoint, message);
    ioc.run();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}