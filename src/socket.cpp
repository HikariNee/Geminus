#include "socket.hpp"
#include "utilities.hpp"
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

Socket::~Socket()
{
  close(this->sock);
}

auto Socket::acceptConn() -> std::optional<Socket>
{
  struct sockaddr addr {};
  socklen_t addrlen = sizeof(addr);
  int sock = accept(this->sock, &addr, &addrlen);

  if (sock == -1)
    return std::nullopt;

  return Socket(sock);
}

auto Socket::getRawSock() -> int
{
  return this->sock;
}
