/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   boost.cc                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 11:00:36 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 11:30:07 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <functional>
#include <iostream>
#include <sys/socket.h>

void print(boost::system::error_code ec, boost::asio::steady_timer *timer,
           int *count) {
  // std::cout << "helloworld" << std::endl;
  using namespace std::chrono_literals;
  if (*count < 3) {
    std::cout << *count << std::endl;
    ++(*count);
    timer->expires_after(1s);
    timer->async_wait(std::bind(&print, std::placeholders::_1, timer, count));
  }
}

int main(int argc, char *argv[]) {
  using namespace boost::asio;
  using namespace std::chrono_literals;
  io_context ioc;
  int count{};
  steady_timer timer(ioc, 1s);
  timer.async_wait(std::bind(&print, std::placeholders::_1, &timer, &count));
  ioc.run();
  return 0;
}
