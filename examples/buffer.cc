/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffer.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 20:27:18 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 21:16:41 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/impl/write.hpp>
#include <ctime>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using boost::asio::ip::tcp;

class shared_const_buffer {
  std::shared_ptr<std::vector<char>> data_;
  boost::asio::const_buffer buffer_;

public:
  explicit shared_const_buffer(const std::string &data)
      : data_(new std::vector<char>(data.begin(), data.end())),
        buffer_(boost::asio::buffer(*data_)) {}
  typedef boost::asio::const_buffer value_type;
  typedef const boost::asio::const_buffer *const_iterator;
  const const_iterator begin() const { return &buffer_; }
  const const_iterator end() const { return &buffer_ + 1; }
};

class session : public std::enable_shared_from_this<session> {
  tcp::socket socket_;

public:
  session(tcp::socket socket) : socket_(std::move(socket)) {}
  void start() { do_write(); }

private:
  void do_write() {
    std::time_t now = std::time(0);
    shared_const_buffer buffer(std::ctime(&now));
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, buffer, [self](boost::system::error_code ec, std::size_t) {});
  }
};

class server {
  tcp::acceptor acceptor_;

public:
  server(boost::asio::io_context &ioc, short port)
      : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }
          do_accept();
        });
  }
};
