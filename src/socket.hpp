#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>

class Socket {
  public:
  Socket()
      : sock(0) { };
  Socket(int fd)
      : sock(fd) { };
  ~Socket();

  auto acceptConn() -> std::optional<Socket>;
  auto getRawSock() -> int;

  private:
  int sock;
};
