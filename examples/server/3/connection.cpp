/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:28:29 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 15:51:27 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "connection.hpp"
#include "request_handler.hpp"
#include <boost/asio/impl/write.hpp>
#include <utility>

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket,
                       request_handler &handler)
    : socket_(std::move(socket)), request_handler_(handler) {}

void connection::start() { do_read(); }

void connection::do_read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      [this, self](const boost::system::error_code ec,
                   std::size_t bytes_transferred) {
        if (!ec) {
          auto [result, _] = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);
          if (result == request_parser::result_type::good) {
            request_handler_.handle_request(request_, reply_);
            do_write();
          } else if (result == request_parser::result_type::bad) {
            reply_ = reply::stock_reply(reply::status_type::bad_request);
            do_write();
          } else {
            do_read();
          }
        }
      });
}
void connection::do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(socket_, reply_.to_buffers(),
                           [this, self](const boost::system::error_code ec,
                                        std::size_t bytes_transferred) {
                             if (!ec) {
                               boost::system::error_code ignored_ec;
                               auto ec1 = socket_.shutdown(
                                   boost::asio::ip::tcp::socket::shutdown_both,
                                   ignored_ec);
                             }
                           });
}

} // namespace server
} // namespace http
