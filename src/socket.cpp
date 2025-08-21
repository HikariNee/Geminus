#include "socket.hpp"
#include "utilities.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

auto Socket::sendData(const std::string& p) -> void
{
  Socket::sendData(std::span<std::byte>(
      reinterpret_cast<std::byte*>(const_cast<char*>(p.data())),
      p.size()));
}

auto Socket::recvData(int bytes) -> std::optional<std::vector<std::byte>>
{
  std::vector<std::byte> buf(bytes);

  if (int i = read(this->sock, static_cast<void*>(buf.data()), bytes); i == -1) {
    printDebug(std::string_view { std::strerror(errno) });
    return std::nullopt;
  };

  if (buf.empty()) return std::nullopt;
  return buf;
};

auto Socket::recvDataAccurately(int bytes) -> std::optional<std::vector<std::byte>>
{
  std::vector<std::byte> buf(bytes);
  size_t bytesRead {};

  while (bytesRead < static_cast<size_t>(bytes)) {
    int i = read(this->sock, static_cast<void*>(buf.data() + bytesRead), bytes - bytesRead);
    if (i == -1) {
      printDebug(std::string_view { std::strerror(errno) });
      return std::nullopt;
    };

    // socket closed while reading.
    if (i == 0) {
      break;
    };

    bytesRead += i;
  }

  buf.resize(bytesRead);

  if (buf.empty()) return std::nullopt;
  return buf;
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


auto bytesToString(const std::span<std::byte> buf) -> std::string
{
  return std::string { reinterpret_cast<const char*>(buf.data()), buf.size() };
}
