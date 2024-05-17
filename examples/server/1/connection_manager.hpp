/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection_manager.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 12:28:24 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 12:31:26 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "connection.hpp"
#include <set>

namespace http {
namespace server {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager {
public:
  connection_manager();
  connection_manager(const connection_manager &) = delete;
  connection_manager &operator=(const connection_manager &) = delete;

  void start(connection_ptr c);
  void stop(connection_ptr c);
  void stop_all();

private:
  /// The managed connections.
  std::set<connection_ptr> connections_;
};

} // namespace server
} // namespace http
