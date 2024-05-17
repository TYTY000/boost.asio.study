/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:27:31 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 10:30:13 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::string make_time() {
  using namespace std;
  time_t now = time(0);
  return ctime(&now);
}

int main(int argc, char *argv[]) {
  try {
    boost::asio::io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), 13);
    tcp::acceptor acceptor(ioc, endpoint);
    while (true) {
      tcp::iostream ostr;
      boost::system::error_code ec;
      acceptor.accept(ostr.socket(), ec);
      if (!ec) {
        ostr << make_time();
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
