#include "connection.hpp"
#include "socket.hpp"
#include "utilities.hpp"
#include <arpa/inet.h>
#include <gnutls/gnutls.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <span>
#include <string>
#include <variant>

enum class IP {
  IPV6,
  IPV4,
  INVALID
};

auto checkIP(const std::string& ip) -> IP
{
  struct in_addr ipv4addr {};
  struct in6_addr ipv6addr {};

  if (inet_pton(AF_INET6, ip.c_str(), &ipv6addr) == 1) {
    return IP::IPV6;
  }

  if (inet_pton(AF_INET, ip.c_str(), &ipv4addr) == 1) {
    return IP::IPV4;
  }

  return IP::INVALID;
}

auto createSocket(const std::string& ip) -> std::optional<Socket>
{
  IP iptype = checkIP(ip);

  // return early if the IP is invalid.
  if (iptype == IP::INVALID) {
    printDebug("IP provided was neither in the IPV4 or IPV6 format");
    return std::nullopt;
  }

  struct addrinfo hints {};
  struct addrinfo* res_ {};

  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_protocol = 0;

  if (int status = getaddrinfo(ip.c_str(), "1965", &hints, &res_); status != 0) {
    printDebug(gai_strerror(status));
    return std::nullopt;
  }

  // wrap res in a unique_ptr so I don't have to explicitly free it all the time.
  std::unique_ptr<struct addrinfo, void (*)(struct addrinfo*)> res(
      res_,
      [](struct addrinfo* ptr) { freeaddrinfo(ptr); });

  for (auto p = res.get(); p != nullptr; p = p->ai_next) {
    int family = p->ai_family;
    int sock = socket(family, p->ai_socktype, p->ai_protocol);
    char ipstr[INET6_ADDRSTRLEN];
    std::variant<sockaddr_in6, sockaddr_in> addr {};

    if (iptype == IP::IPV6) {
      addr = *reinterpret_cast<struct sockaddr_in6*>(p->ai_addr);
    } else {
      addr = *reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
    }

    // if a socket could not be created, try the other structures.
    if (sock == -1) {
      printDebug(std::string_view { std::strerror(errno) });
      continue;
    }

    // why the fuck do I have to do it this way man.
    int opt = 1;
    if (int i = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, static_cast<const void*>(&opt), sizeof(opt)); i == -1) {
      printDebug(std::string_view { std::strerror(errno) });
      return std::nullopt;
    }

    // socket was successfully created, try binding it.
    if (int i = bind(sock, p->ai_addr, p->ai_addrlen); i == -1) {
      printDebug(std::string_view { std::strerror(errno) });
      ;
      return std::nullopt;
    }

    // make it a apassive socket
    if (int i = listen(sock, 1024); i == -1) {
      printDebug(std::string_view { std::strerror(errno) });
      return std::nullopt;
    }

    // Apply a function to convert the ip address to a string.
    bool success = std::visit([family, &ipstr](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;

      if constexpr (std::is_same_v<T, sockaddr_in6>) {
        if (const char* s = inet_ntop(family, static_cast<const void*>(&(arg.sin6_addr)), ipstr, sizeof(ipstr)); s == nullptr) {
          printDebug(std::string_view { std::strerror(errno) });
          return false;
        };
      }

      if constexpr (std::is_same_v<T, sockaddr_in>) {
        if (const char* s = inet_ntop(family, static_cast<const void*>(&(arg.sin_addr)), ipstr, sizeof(ipstr)); s == nullptr) {
          printDebug(std::string_view { std::strerror(errno) });
          return false;
        };
      }

      return true;
    },
        addr);

    if (success) {
      printDebug(std::format("Socket created, bound and set to 'listen' at \"{}\"", std::string { ipstr }));
      return Socket(sock);
    }
  }

  return std::nullopt;
}
