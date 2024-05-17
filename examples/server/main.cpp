/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 11:58:45 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 16:05:59 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file_handler.hpp"
#include "server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#define BOOST_ASIO_NO_DEPRECATED 1
int main(int argc, char *argv[]) {
  try {
    // Check command line arguments.
    if (argc != 4) {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    boost::asio::io_context io_context;

    // Launch the initial server coroutine.
    http::server::server(io_context, argv[1], argv[2],
                         http::server::file_handler(argv[3]))();

    // Wait for signals indicating time to shut down.
    boost::asio::signal_set signals(io_context);
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(
        [&io_context](boost::system::error_code, int) { io_context.stop(); });

    // Run the server.
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
