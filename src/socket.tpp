#pragma once
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string_view>
#include <sys/socket.h>
#include "utilities.hpp"

template<typename T>
inline auto Socket::sendData(std::span<T> obj) -> void
{
  if (int i = send(this->sock, static_cast<const void*>(obj.data()), obj.size(), MSG_NOSIGNAL); i == -1) {
   if (errno == EPIPE) return;
    printDebug(std::string_view { std::strerror(errno) });
  }
};
