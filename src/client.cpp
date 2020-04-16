#include <array>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <string>

namespace po = boost::program_options;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

int main(int argc, char *argv[]) {
  try {
    std::string host;
    unsigned short port;
    std::string nickname;

    po::options_description generic("Generic options");
    generic.add_options()("help,h", "show this message");

    po::options_description config("Configuration");
    config.add_options()(
        "host", po::value<std::string>(&host)->default_value("localhost"),
        "server ip address")(
        "port,p", po::value<unsigned short>(&port)->default_value(13),
        "connection port")(
        "name,n", po::value<std::string>(&nickname)->default_value("User"),
        "your nickname");

    po::variables_map vm;
    po::options_description cmdline_options = generic.add(config);
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

    if (vm.count("help")) {
      std::cout << cmdline_options;
      return 0;
    }

    po::notify(vm);

    asio::io_context ioc;
    tcp::resolver resolver(ioc);
    auto endpoints = resolver.resolve(host, std::to_string(port));

    tcp::socket socket(ioc);
    asio::connect(socket, endpoints);

    while (true) {
      std::array<char, 128> buf{""};
      error_code error;
      socket.read_some(asio::buffer(buf), error);

      if (error == asio::error::eof)
        break; // Connection closed cleanly by peer.
      if (error)
        throw boost::system::system_error(error); // Some other error.

      std::cout << buf.data();
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}