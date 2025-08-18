#pragma once

#include <netdb.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include "socket.hpp"

auto createSocket(const std::string& = "127.0.0.1") -> std::optional<Socket>;
auto initializeTLSAndHandshake(int) -> void;
