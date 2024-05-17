/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cc                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: TYTY000 <767280080@qq.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 08:18:29 by TYTY000           #+#    #+#             */
/*   Updated: 2024/05/14 09:59:00 by TYTY000          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "usage: showip hostname\n");
    return 1;
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  printf("IP addresses for %s:\n\n", argv[1]);

  for (p = res; p != nullptr; p = p->ai_next) {
    void *addr;
    std::string ipver;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) { // IPv4
      struct sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(p->ai_addr);
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else { // IPv6
      struct sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6 *>(p->ai_addr);
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    std::cout << ipver << ":\t" << ipstr << '\n';
  }

  freeaddrinfo(res); // free the linked list

  return 0;

  // struct sockaddr_in sa;
  // struct sockaddr_in6 sa6; // IPv6
  // sa.sin_addr.s_addr = INADDR_ANY;
  // sa.sin_family = AF_INET;
  // sa.sin_port = htons(10002);

  // char ip4[INET_ADDRSTRLEN]; // space to hold the IPv4 string
  // inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
  // printf("The IPv4 address is: %s\n", ip4);

  // struct addrinfo hints {
  //   .ai_flags = AI_PASSIVE, .ai_family = AF_UNSPEC, .ai_socktype =
  //   SOCK_STREAM,
  // };
  // struct addrinfo *binginfo;
  // if (int status = getaddrinfo("bing.com", "https", &hints, &binginfo) != 0)
  // {
  //   std::cerr << "getaddrinfo error: " << gai_strerror(status) << '\n';
  //   exit(1);
  // }

  // char ipstr[INET6_ADDRSTRLEN];
  // void *addr;
  // for (int i = 0; i < 5; ++i) {
  //   if (binginfo[i].ai_family == AF_INET) { // IPv4
  //     struct sockaddr_in *ipv4 =
  //         reinterpret_cast<sockaddr_in *>(binginfo->ai_addr);
  //     addr = &(ipv4->sin_addr);
  //   } else { // IPv6
  //     struct sockaddr_in6 *ipv6 =
  //         reinterpret_cast<sockaddr_in6 *>(binginfo->ai_addr);
  //     addr = &(ipv6->sin6_addr);
  //   }
  //   inet_ntop(binginfo->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
  //   std::cout << "IP address: " << ipstr << '\n';
  // }
  // freeaddrinfo(binginfo); // free the linked-list
  // return 0;
}
