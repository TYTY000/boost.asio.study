/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:51:16 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 15:48:31 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "connection.hpp"
#include "request_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <csignal>
#include <memory>
#include <thread>
#include <utility>

namespace http {
namespace server {

server::server(const std::string &address, const std::string &port,
               const std::string &doc_root, std::size_t threads)
    : ioc_(1), signals_(ioc_), acceptor_(ioc_), request_handler_(doc_root),
      thread_pool_size_(threads) {
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
  signals_.add(SIGQUIT);

  do_await_stop();
  boost::asio::ip::tcp::resolver resolver(ioc_);
  boost::asio::ip::tcp::endpoint endpoint =
      *resolver.resolve(address, port).begin();
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

void server::run() {
  std::vector<std::thread> threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
    threads.emplace_back([this] { ioc_.run(); });

  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i].join();
}

void server::do_accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!acceptor_.is_open()) {
      return;
    }
    if (!ec) {
      std::make_shared<connection>(std::move(socket), request_handler_)
          ->start();
      do_accept();
    }
  });
}

void server::do_await_stop() {
  signals_.async_wait(
      [this](boost::system::error_code ec, int signo) { ioc_.stop(); });
}
} // namespace server
} // namespace http
