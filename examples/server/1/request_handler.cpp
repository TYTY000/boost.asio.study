/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_handler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:00:46 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 14:15:32 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace http {
namespace server {

request_handler::request_handler(const std::string &doc_root)
    : doc_root_(doc_root) {}

// 这段代码是一个URL解码函数，它的作用是将URL中的特殊字符转换回原始字符。
bool request_handler::url_decode(const std::string &in, std::string &out) {
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return true;
}

void request_handler::handle_request(const request &req, reply &rep) {

  std::string request_path;
  if (!url_decode(req.uri, request_path)) {
    rep = reply::stock_reply(reply::status_type::bad_request);
    return;
  }
  // Request path must be absolute and not contain "..".
  if (request_path.empty() or request_path[0] != '/' or
      request_path.find("..") != std::string::npos) {
    rep = reply::stock_reply(reply::status_type::bad_request);
    return;
  }
  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/') {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is) {
    rep = reply::stock_reply(reply::status_type::bad_request);
    return;
  }

  rep.status = reply::status_type::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0) {
    rep.content.append(buf, is.gcount());
    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = mime_types::extension_to_type(extension);
  }
}

} // namespace server
} // namespace http
