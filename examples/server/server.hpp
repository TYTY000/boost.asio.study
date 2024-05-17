/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:52:36 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 16:05:10 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
// #include "request_handler.hpp"
#include <boost/asio.hpp>
#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio/io_context.hpp>

namespace http {
namespace server {

class server {
  boost::asio::io_context ioc_;
  boost::asio::signal_set signals_;
  boost::asio::ip::tcp::acceptor acceptor_;
  // request_handler request_handler_;
  std::size_t thread_pool_size_;

public:
  server(const server &) = delete;
  server &operator=(const server &) = delete;
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string &address, const std::string &port,
                  const std::string &doc_root, std::size_t threads);

  void run();

private:
  void do_accept();
  void do_await_stop();
};

} // namespace server
} // namespace http
