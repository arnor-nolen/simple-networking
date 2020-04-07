#include <array>
#include <boost/asio.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    const std::string host = "localhost";
    const short port = 13;

    namespace asio = boost::asio;
    using tcp = asio::ip::tcp;
    using error_code = boost::system::error_code;

    asio::io_context ioc;
    tcp::resolver resolver(ioc);
    auto endpoints = resolver.resolve(host, std::to_string(port));
    tcp::socket socket(ioc);
    asio::connect(socket, endpoints);

    while (true) {
      std::array<char, 128> buf;
      error_code error;
      size_t len = socket.read_some(asio::buffer(buf), error);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      std::cout << buf.data() << "\n";
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}