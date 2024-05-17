/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 09:55:42 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 09:57:43 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <iostream>

using boost::asio::ip::udp;

enum { max_length = 1024 };

void server(boost::asio::io_context &ioc, unsigned short port) {
  udp::socket socket(ioc, udp::endpoint(udp::v4(), port));
  while (true) {
    char data[max_length];
    udp::endpoint sender_endpoint;
    auto length = socket.receive_from(boost::asio::buffer(data, max_length),
                                      sender_endpoint);
    socket.send_to(boost::asio::buffer(data, length), sender_endpoint);
  }
}

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: blocking_udp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server(io_context, std::atoi(argv[1]));
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
