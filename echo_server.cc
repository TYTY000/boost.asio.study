/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_server.cc                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 11:32:05 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 19:18:10 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <array>
#include <boost/asio/io_context.hpp>
#include <cstdlib>
#include <memory>
#include <utility>
#define BOOST_ASIO_NO_DEPRECATED 1
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING 1
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
using namespace boost::asio;
using namespace boost::asio::ip;
enum { BUFF_SIZE = 1024 };

class Session : public std::enable_shared_from_this<Session> {
  tcp::socket _socket;
  std::array<char, BUFF_SIZE> _buffer;

public:
  Session(tcp::socket socket) : _socket(std::move(socket)) {}

  void write(size_t length) {
    auto self(shared_from_this());
    async_write(_socket, buffer(_buffer, length),
                [this, self](boost::system::error_code ec, size_t length) {
                  if (!ec) {
                    read();
                  }
                });
  }

  void read() {
    auto self(shared_from_this());
    _socket.async_read_some(
        buffer(_buffer),
        [this, self](boost::system::error_code ec, size_t length) {
          if (!ec) {
            write(length);
          }
        });
  }
};

class Server {
  tcp::acceptor _acceptor;

private:
  void accept() {
    _acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<Session>(std::move(socket))->read();
          }
          accept();
        });
  }

public:
  Server(boost::asio::io_context &ioc, uint16_t port)
      : _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
    accept();
  }
};

void session(tcp::socket socket) {
  try {
    while (true) {
      boost::array<char, BUFF_SIZE> data;
      boost::system::error_code ec;
      size_t length = socket.read_some(buffer(data), ec);
      if (ec == boost::asio::error::eof) {
        std::cout << "reached eof!" << '\n';
        break;
      } else if (ec) {
        throw boost::system::system_error(ec);
      }
      boost::asio::write(socket, buffer(data, length));
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception:\t" << e.what() << '\n';
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }
  unsigned short port = std::atoi(argv[1]);
  io_context ioc;
  Server server{ioc, port};
  ioc.run();
  // tcp::acceptor ac(ioc, tcp::endpoint(tcp::v4(), port));

  // try {
  //   while (true) {
  //     session(ac.accept());
  //   }
  // } catch (const std::exception &e) {
  //   std::cerr << "Exception:\t" << e.what() << '\n';
  // }
  return 0;
}
