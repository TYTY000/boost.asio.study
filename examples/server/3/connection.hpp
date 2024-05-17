/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:17:31 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 15:52:58 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"
#include <array>
#include <boost/asio.hpp>
#include <memory>

namespace http {
namespace server {

// class connection_manager;

class connection : public std::enable_shared_from_this<connection> {

  boost::asio::ip::tcp::socket socket_;
  // connection_manager &connection_manager_;
  request_handler &request_handler_;
  std::array<char, 8192> buffer_;
  request request_;
  request_parser request_parser_;
  reply reply_;

public:
  connection(const connection &) = delete;
  connection &operator=(const connection &) = delete;

  explicit connection(boost::asio::ip::tcp::socket socket,
                      request_handler &handler);

  void start();

private:
  void do_read();
  void do_write();
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http
