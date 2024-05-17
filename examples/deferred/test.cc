/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 08:29:05 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 08:32:22 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>

using boost::asio::deferred;

int main(int argc, char *argv[]) {
  using namespace std::chrono_literals;
  boost::asio::io_context ioc;
  boost::asio::steady_timer timer(ioc);
  timer.expires_after(1s);

  auto deferred_op =
      timer.async_wait(deferred([&](boost::system::error_code ec) {
        std::cout << "timer expired " << ec.message() << '\n';
        timer.expires_after(1s);
        return timer.async_wait(deferred);
      }));

  std::move(deferred_op)([](boost::system::error_code ec) {
    std::cout << "second timer wait finished: " << ec.message() << "\n";
  });

  ioc.run();

  return 0;
}
