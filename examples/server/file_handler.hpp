/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_handler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:18:56 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 12:20:51 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

namespace http {
namespace server {
struct reply;
struct request;
class request_handler {
  std::string doc_root_;

public:
  explicit request_handler(const std::string &doc_root);

  request_handler(const request_handler &) = delete;
  request_handler &operator=(const request_handler &) = delete;

  void handle_request(const request &req, reply &rep);

private:
  static bool url_decode(const std::string &in, std::string &out);
};
} // namespace server
} // namespace http
