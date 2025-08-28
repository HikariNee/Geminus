module;

#include <sys/socket.h>
#include <unistd.h>

export module Socket;
import std;
import Utils;

export class Socket;
export using sockaddr = struct sockaddr;
export enum class IP {
  IPV6,
  IPV4,
  INVALID
};

class Socket {
  public:
  Socket()
      : sock(0) { };
  Socket(int fd, IP iptype)
      : sock(fd), family(iptype) { };
  //~Socket();

  std::optional<std::pair<Socket, struct sockaddr>> acceptConn();
  int getRawSock();
  IP getFamily();

  private:
  int sock;
  IP  family;
};

auto Socket::acceptConn() -> std::optional<std::pair<Socket, struct sockaddr>>
{
  struct sockaddr addr {};
  socklen_t addrlen = sizeof(addr);
  int sock = accept(this->sock, &addr, &addrlen);

  if (sock == -1)
    return std::nullopt;

  return std::make_pair(Socket(sock, this->family), addr);
}

auto Socket::getRawSock() -> int
{
  return this->sock;
}

auto Socket::getFamily() -> IP
{
  return family;
}
