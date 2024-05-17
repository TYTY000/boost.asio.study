/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_client.cc                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 11:54:23 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 19:18:44 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <array>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <sys/socket.h>
using boost::asio::ip::tcp;
enum { BUFF_SIZE = 1024 };

class Client {
  tcp::socket _socket;
  tcp::resolver _resolver;
  std::array<char, BUFF_SIZE> _buffer;
  char cin_buf_[BUFF_SIZE];

public:
  Client(boost::asio::io_context &ioc, const std::string &host,
         const std::string &port)
      : _socket(ioc), _resolver(ioc) {
    _resolver.async_resolve(tcp::v4(), host, port,
                            std::bind(&Client::OnResolve, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
    auto endpoints = _resolver.resolve(tcp::v4(), host, port);
    boost::asio::async_connect(_socket, endpoints,
                               std::bind(&Client::OnConnect, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  void OnResolve(boost::system::error_code ec,
                 tcp::resolver::results_type endpoints) {
    if (ec) {
      std::cerr << "Resolve: " << ec.message() << std::endl;
    } else {
      boost::asio::async_connect(_socket, endpoints,
                                 std::bind(&Client::OnConnect, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
    }
  }

  void OnConnect(boost::system::error_code ec, tcp::endpoint endpoint) {
    if (ec) {
      std::cout << "Connect failed: " << ec.message() << std::endl;
      _socket.close();
    } else {
      DoWrite();
    }
  }

  void DoWrite() {
    std::size_t len = 0;
    do {
      std::cout << "Enter message: ";
      std::cin.getline(cin_buf_, BUFF_SIZE);
      len = strlen(cin_buf_);
    } while (len == 0);

    boost::asio::async_write(
        _socket, boost::asio::buffer(cin_buf_, len),
        std::bind(&Client::OnWrite, this, std::placeholders::_1));
  }

  void OnWrite(boost::system::error_code ec) {
    if (!ec) {
      std::cout << "Reply is: ";

      _socket.async_read_some(boost::asio::buffer(_buffer),
                              std::bind(&Client::OnRead, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
    }
  }

  void OnRead(boost::system::error_code ec, std::size_t length) {
    if (!ec) {
      std::cout.write(_buffer.data(), length);
      std::cout << std::endl;
      // 如果想继续下一轮，可以在这里调用 DoWrite()。
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
    return 1;
  }

  std::string host{argv[1]}, port{argv[2]};
  boost::asio::io_context ioc;

  // Client client{ioc, host, port};
  // ioc.run();
  try {
    tcp::resolver resolver{ioc};
    auto endpoints = resolver.resolve(tcp::v4(), host, port);
    tcp::socket socket{ioc};
    boost::asio::connect(socket, endpoints);

    char request[BUFF_SIZE];
    size_t request_length{0};
    do {
      std::cout << "Msg: ";
      std::cin.getline(request, BUFF_SIZE);
      request_length = std::strlen(request);
    } while (request_length == 0);
    boost::asio::write(socket, boost::asio::buffer(request, request_length));

    std::cout << "reply: ";
    // char reply[BUFF_SIZE];
    // size_t reply_length = 0;
    // boost::asio::read(socket, boost::asio::buffer(reply, reply_length));
    // std::cout.write(reply, reply_length);
    size_t total_reply_length{};
    while (true) {
      std::array<char, BUFF_SIZE> reply;
      size_t reply_length = socket.read_some(boost::asio::buffer(reply));
      std::cout.write(reply.data(), reply_length);
      total_reply_length += reply_length;
      if (total_reply_length == request_length) {
        break;
      }
    }
    std::cout << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
