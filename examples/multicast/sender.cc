/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sender.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 10:47:43 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/16 11:58:43 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

constexpr short multicast_port{30001};
static int max_msg_cnt;
static int total_cnt;

using namespace std::chrono_literals;
class sender {
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::steady_timer timer_;
  std::string msg_;
  static std::mutex mutex_;
  static int msg_cnt_;

public:
  sender(boost::asio::io_context &ioc,
         const boost::asio::ip::address &multicast_addr)
      : endpoint_(multicast_addr, multicast_port),
        socket_(ioc, endpoint_.protocol()), timer_(ioc) {
    do_send();
  }

private:
  void do_send() {
    std::ostringstream os;
    std::lock_guard<std::mutex> lock(mutex_);
    os << "Message " << msg_cnt_++;
    msg_ = os.str();
    socket_.async_send_to(
        boost::asio::buffer(msg_), endpoint_,
        [this](boost::system::error_code ec, std::size_t length) {
          if (!ec and msg_cnt_ <= total_cnt) {
            do_timeout();
          }
        });
  }

  void do_timeout() {
    timer_.expires_after(100ms);
    timer_.async_wait([this](boost::system::error_code ec) {
      if (!ec) {
        do_send();
      }
    });
  }
};

int sender::msg_cnt_ = 1;
std::mutex sender::mutex_;
int main(int argc, char *argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: sender <multicast_address> <threads>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    sender 239.255.0.1 10\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    sender ff31::8000:1234 10\n";
      return 1;
    }

    boost::asio::io_context io_context;
    std::vector<std::thread> threads;
    int n = std::stoi(argv[2]);
    int thread_num = std::sqrt(n);
    max_msg_cnt = thread_num + 1;
    total_cnt = n - thread_num + 1;
    std::cout << total_cnt;
    for (int i = 0; i < thread_num; ++i) {
      threads.emplace_back([&io_context, &argv]() {
        sender s(io_context, boost::asio::ip::make_address(argv[1]));
        io_context.run();
      });
    }
    for (auto &thread : threads) {
      thread.join();
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
