/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cc                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 19:27:52 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 19:50:00 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <boost/asio.hpp>
#include <boost/asio/bind_allocator.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
using boost::asio::ip::tcp;

class handler_memory {
  typename std::aligned_storage_t<1024> storage_;
  bool in_use_;

public:
  handler_memory() : in_use_(false) {}
  handler_memory(const handler_memory &) = delete;
  handler_memory &operator=(const handler_memory &) = delete;

  void *allocate(std::size_t size) {
    if (!in_use_ and size < sizeof(storage_)) {
      in_use_ = true;
      return &storage_;
    } else {
      return ::operator new(size);
    }
  }

  void deallocate(void *ptr) {
    if (ptr == &storage_) {
      in_use_ = false;
    } else {
      ::operator delete(ptr);
    }
  }
};

template <typename T> class handler_allocater {
  template <typename> friend class handler_allocater;
  handler_memory &memory_;

public:
  using value_type = T;
  explicit handler_allocater(handler_memory &m) : memory_(m) {}
  template <typename U>
  handler_allocater(const handler_allocater<U> &other) noexcept
      : memory_(other.memory_) {}
  bool operator==(const handler_allocater &other) const noexcept {
    return other.memory_ == this->memory_;
  }
  bool operator!=(const handler_allocater &other) const noexcept {
    return other.memory_ != this->memory_;
  }

  T *allocate(std::size_t n) const {
    return static_cast<T *>(memory_.allocate(n * sizeof(T)));
  }
  void deallocate(T *p, std::size_t) const { return memory_.deallocate(p); }
};

class session : public std::enable_shared_from_this<session> {
  tcp::socket socket_;
  std::array<char, 1024> data_;
  handler_memory handler_memory;

public:
  session(tcp::socket socket) : socket_(std::move(socket)) {}
  void start() { do_read(); }
  void do_read() {
    auto self{shared_from_this()};
    socket_.async_read_some(
        boost::asio::buffer(data_),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            do_write(length);
          }
        });
  }
  void do_write(std::size_t length) {
    auto self{shared_from_this()};
    boost::asio::async_write(
        socket_, boost::asio::buffer(data_, length),
        boost::asio::bind_allocator(
            handler_allocater<int>(handler_memory),
            [this, self](boost::system::error_code ec, std::size_t length) {
              if (!ec) {
                do_read();
              }
            }));
  }
};

class server {
  tcp::acceptor acceptor_;
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }
        });
  }

public:
  server(boost::asio::io_context &ioc, short port)
      : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }
};

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " <port>\n";
      return 1;
    }
    boost::asio::io_context ioc;
    server s(ioc, std::atoi(argv[1]));
    ioc.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return 0;
}
