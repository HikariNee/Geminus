#pragma once

#include <span>
#include <vector>
#include <string>
#include <optional>

class Socket {
public:
  Socket() : sock(0) {};
  Socket(int fd) : sock(fd) {};
  auto sendData(const std::string&) -> void;
  template<typename T>
  auto sendData(std::span<T>) -> void;
  auto recvData(int) -> std::optional<std::vector<std::byte>>;
  auto recvDataAccurately(int) -> std::optional<std::vector<std::byte>>;
  auto acceptConn() -> std::optional<Socket>;
  auto getRawSock() -> int;

private:
  int sock;
};
auto bytesToString(const std::span<std::byte>) -> std::string;
#include "socket.tpp"
