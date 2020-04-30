#include "pch.h"
#include <future>

namespace po = boost::program_options;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

struct client {
  client(asio::io_context &ioc, const tcp::resolver::results_type &endpoints,
         const std::string &nickname)
      : socket_(ioc), nickname_(nickname) {
    connect(endpoints);
  }

  void connect(const tcp::resolver::results_type &endpoints) {
    asio::async_connect(
        socket_, endpoints,
        [this](const error_code &error, const tcp::endpoint &endpoint) {
          client::handle_connect(error, endpoint);
        });
  }

  void read() {
    socket_.async_read_some(asio::buffer(buf_), [this](const error_code &error,
                                                       const size_t &size) {
      client::handle_read(error, size);
    });
  }

  void send(const std::string &message) {
    asio::async_write(socket_, asio::buffer(message),
                      [this](const error_code &error, const size_t &size) {
                        client::handle_write(error, size);
                      });
  }

private:
  void handle_connect(const error_code &error, const tcp::endpoint &) {
    if (!error) {
      std::cout << "Successfully connected!\n";
      send(nickname_);
      read();
    } else
      std::cerr << "Error: " << error.message() << "\n";
  }

  void handle_read(const error_code &error, const size_t &size) {
    if (!error) {
      std::string message(buf_.data(), size);
      std::cout << message;
      read();
    } else if (error == asio::error::eof)
      ; // silently shut down
    else {
      std::cerr << "Error: " << error.message() << "\n";
    }
  }

  void handle_write(const error_code &error, const size_t &) {
    if (error)
      std::cerr << "Error: " << error.message() << "\n";
  }

  tcp::socket socket_;
  std::array<char, 128> buf_{""};
  std::string nickname_;
};

void console_read(client &c) {
  std::string message;
  std::getline(std::cin, message);
  c.send(message);
  console_read(c);
}

int main(int argc, char *argv[]) {
  try {
    std::string host;
    unsigned short port;
    std::string nickname;
    bool read_only;

    po::options_description generic("Generic options");
    generic.add_options()("help", "show this message");

    po::options_description config("Configuration");
    config.add_options()(
        "host,h", po::value<std::string>(&host)->default_value("localhost"),
        "server ip address")(
        "port,p", po::value<unsigned short>(&port)->default_value(13),
        "connection port")("name,n",
                           po::value<std::string>(&nickname)->required(),
                           "your nickname")(
        "read-only,r", po::bool_switch(&read_only)->default_value(false),
        "read-only mode");

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

    client c(ioc, endpoints, nickname);

    std::future<void> input_promise;

    if (!read_only) {
      input_promise =
          std::async(std::launch::async, [&c]() { console_read(c); });
    }

    ioc.run();

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}