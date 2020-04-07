#include <boost/asio.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    const short port = 13;
    const std::string message = "Hello, world!";

    namespace asio = boost::asio;
    using tcp = asio::ip::tcp;
    using error_code = boost::system::error_code;

    asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), port);
    tcp::acceptor acceptor(ioc, endpoint);

    while (true) {
      tcp::socket socket(ioc);
      error_code error;

      acceptor.accept(socket);
      std::cout << "Connection accepted!\n";
      asio::write(socket, asio::buffer(message), error);
      std::cout << "Message sent!\n\n";
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}