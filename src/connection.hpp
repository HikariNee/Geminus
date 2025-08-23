#pragma once
#include "socket.hpp"

auto createSocket(const std::string& = "127.0.0.1") -> std::optional<Socket>;
