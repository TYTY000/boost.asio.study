/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   receiver.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 10:25:02 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/16 13:00:45 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <iostream>

constexpr short multicast_port{30001};

class receiver {
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint endpoint_;
  std::array<char, 1024> buffer_;

public:
  receiver(boost::asio::io_context &ioc,
           const boost::asio::ip::address &listen_addr,
           const boost::asio::ip::address &multicast_addr)
      : socket_(ioc) {
    boost::asio::ip::udp::endpoint listen_ep{listen_addr, multicast_port};
    socket_.open(listen_ep.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_ep);

    socket_.set_option(boost::asio::ip::multicast::join_group(multicast_addr));
    do_receive();
  }

private:
  void do_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(buffer_), endpoint_,
        [this](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::cout.write(buffer_.data(), length);
            std::cout << '\n';
            do_receive();
          }
        });
  }
};

int main(int argc, char *argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: receiver <listen_address> <multicast_address>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 239.255.0.1\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 ff31::8000:1234\n";
      return 1;
    }

    boost::asio::io_context io_context;
    receiver r(io_context, boost::asio::ip::make_address(argv[1]),
               boost::asio::ip::make_address(argv[2]));
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  using namespace boost;
  boost::asio::io_service ios;
  asio::ip::tcp protocol = asio::ip::tcp::v4();
  asio::ip::tcp::socket sock(ios, protocol);
  boost::system::error_code ec;
  return 0;
}
