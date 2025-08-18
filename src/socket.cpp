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

auto Socket::sendData(std::span<std::byte> obj) -> void
{
  if (int i = send(this->sock, static_cast<void*>(obj.data()), obj.size(), 0); i == -1) {
    if (errno == EPIPE) {
      close(this->sock);
    }
    printDebug(std::string_view { std::strerror(errno) });
  }
};

auto Socket::recvData(int bytes) -> std::optional<std::vector<std::byte>>
{
  std::vector<std::byte> buf {};
  buf.resize(bytes + 1);
  if (int i = read(this->sock, static_cast<void*>(buf.data()), bytes); i == -1) {
    printDebug(std::string_view { std::strerror(errno) });
    return std::nullopt;
  }

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

auto bytesToString(const std::span<std::byte> buf) -> std::string
{
  return std::string { reinterpret_cast<const char*>(buf.data()), buf.size() };
}
