/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection_manager.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 14:49:28 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/15 14:50:49 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "connection_manager.hpp"

namespace http {
namespace server {
connection_manager::connection_manager() {}

void connection_manager::start(connection_ptr c) {
  connections_.insert(c);
  c->start();
}
void connection_manager::stop(connection_ptr c) {
  connections_.erase(c);
  c->stop();
}
void connection_manager::stop_all() {
  for (auto c : connections_) {
    c->stop();
  }
  connections_.clear();
}
} // namespace server
} // namespace http
