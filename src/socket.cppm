module;

#include <sys/socket.h>
#include <unistd.h>

export module Socket;
import std;
import Utils;

export class Socket;

class Socket {
  public:
  Socket()
      : sock(0) { };
  Socket(int fd)
      : sock(fd) { };
  //~Socket();

  std::optional<Socket> acceptConn();
  int getRawSock();

  private:
  int sock;
};

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
