/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 09:58:07 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 10:11:26 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>
enum { max_length = 1024 };
using boost::asio::ip::udp;
int main(int argc, char *argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: blocking_udp_echo_client <host> <port>\n";
      return 1;
    }
    boost::asio::io_context ioc;
    udp::socket s(ioc, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(ioc);
    udp::resolver::results_type endpoint =
        resolver.resolve(udp::v4(), argv[1], argv[2]);

    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
    size_t request_length = std::strlen(request);
    s.send_to(boost::asio::buffer(request, request_length), *endpoint.begin());

    char reply[max_length];
    udp::endpoint sender_endpoint;
    size_t reply_length = s.receive_from(
        boost::asio::buffer(reply, reply_length), sender_endpoint);

    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
