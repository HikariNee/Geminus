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
  auto sendData(std::span<std::byte>) -> void;
  auto recvData(int) -> std::optional<std::vector<std::byte>>;
  auto acceptConn() -> std::optional<Socket>;

private:
  int sock;
};


auto bytesToString(const std::span<std::byte>) -> std::string;
