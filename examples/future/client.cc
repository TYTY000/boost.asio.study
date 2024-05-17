/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:16:52 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 10:24:00 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <cstddef>
#include <future>
#include <iostream>

using boost::asio::ip::udp;

void get_daytime(boost::asio::io_context &ioc, const char *hostname) {
  try {
    udp::resolver resolver(ioc);
    std::future<udp::resolver::results_type> endpoints = resolver.async_resolve(
        udp::v4(), hostname, "daytime", boost::asio::use_future);
    udp::socket socket(ioc, udp::v4());
    std::array<char, 1> send_buf = {{0}};

    std::future<std::size_t> send_length =
        socket.async_send_to(boost::asio::buffer(send_buf),
                             *endpoints.get().begin(), boost::asio::use_future);
    send_length.get();
    std::array<char, 256> recv_buf;

    udp::endpoint sender_endpoint;
    std::future<std::size_t> recv_length =
        socket.async_receive_from(boost::asio::buffer(recv_buf),
                                  sender_endpoint, boost::asio::use_future);

    std::cout.write(recv_buf.data(), recv_length.get());
  } catch (boost::system::system_error &e) {
    std::cerr << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: daytime_client <host>" << std::endl;
      return 1;
    }

    // We run the io_context off in its own thread so that it operates
    // completely asynchronously with respect to the rest of the program.
    boost::asio::io_context io_context;
    auto work = boost::asio::make_work_guard(io_context);
    std::thread thread([&io_context]() { io_context.run(); });

    get_daytime(io_context, argv[1]);

    io_context.stop();
    thread.join();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
