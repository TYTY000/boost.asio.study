/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:17:02 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 14:27:48 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_parser.hpp"
#include "request.hpp"

namespace http {
namespace server {

request_parser::request_parser() : state_(state::method_start) {}

void request_parser::reset() { state_ = state::method_start; }

request_parser::result_type request_parser::consume(request &req, char input) {
  switch (state_) {
  case state::method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      state_ = state::method;
      req.method.push_back(input);
      return result_type::indeterminate;
    }
  case state::method:
    if (input == ' ') {
      state_ = state::uri;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.method.push_back(input);
      return result_type::indeterminate;
    }
  case state::uri:
    if (input == ' ') {
      state_ = state::http_version_h;
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      req.uri.push_back(input);
      return result_type::indeterminate;
    }
  case state::http_version_h:
    if (input == 'H') {
      state_ = state::http_version_t_1;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_t_1:
    if (input == 'T') {
      state_ = state::http_version_t_2;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_t_2:
    if (input == 'T') {
      state_ = state::http_version_p;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_p:
    if (input == 'P') {
      state_ = state::http_version_slash;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_slash:
    if (input == '/') {
      req.http_version_major = 0;
      req.http_version_minor = 0;
      state_ = state::http_version_major_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_major_start:
    if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      state_ = state::http_version_major;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_major:
    if (input == '.') {
      state_ = state::http_version_minor_start;
      return result_type::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_major = req.http_version_major * 10 + input - '0';
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_minor_start:
    if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      state_ = state::http_version_minor;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::http_version_minor:
    if (input == '\r') {
      state_ = state::expecting_newline_1;
      return result_type::indeterminate;
    } else if (is_digit(input)) {
      req.http_version_minor = req.http_version_minor * 10 + input - '0';
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::expecting_newline_1:
    if (input == '\n') {
      state_ = state::header_line_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::header_line_start:
    if (input == '\r') {
      state_ = state::expecting_newline_3;
      return result_type::indeterminate;
    } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
      state_ = state::header_lws;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.headers.push_back(header());
      req.headers.back().name.push_back(input);
      state_ = state::header_name;
      return result_type::indeterminate;
    }
  case state::header_lws:
    if (input == '\r') {
      state_ = state::expecting_newline_2;
      return result_type::indeterminate;
    } else if (input == ' ' || input == '\t') {
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      state_ = state::header_value;
      req.headers.back().value.push_back(input);
      return result_type::indeterminate;
    }
  case state::header_name:
    if (input == ':') {
      state_ = state::space_before_header_value;
      return result_type::indeterminate;
    } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
      return result_type::bad;
    } else {
      req.headers.back().name.push_back(input);
      return result_type::indeterminate;
    }
  case state::space_before_header_value:
    if (input == ' ') {
      state_ = state::header_value;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::header_value:
    if (input == '\r') {
      state_ = state::expecting_newline_2;
      return result_type::indeterminate;
    } else if (is_ctl(input)) {
      return result_type::bad;
    } else {
      req.headers.back().value.push_back(input);
      return result_type::indeterminate;
    }
  case state::expecting_newline_2:
    if (input == '\n') {
      state_ = state::header_line_start;
      return result_type::indeterminate;
    } else {
      return result_type::bad;
    }
  case state::expecting_newline_3:
    return (input == '\n') ? result_type::good : result_type::bad;
  default:
    return result_type::bad;
  }
}

bool request_parser::is_char(int c) { return c >= 0 and c <= 127; }

bool request_parser::is_ctl(int c) {
  return (c >= 0 and c <= 31) or (c == 127);
}

bool request_parser::is_tspecial(int c) {
  switch (c) {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c) { return c >= '0' and c <= '9'; }

} // namespace server
} // namespace http
