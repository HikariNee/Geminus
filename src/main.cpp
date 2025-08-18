#include "connection.hpp"
#include "utilities.hpp"
#include <unistd.h>
#include <botan/block_cipher.h>
#include <botan/hex.h>
#include <iostream>

auto main() -> int
{
  auto block = Botan::hex_decode_locked("00112233445566778899AABBCCDDEEFF");
  // We want to crash here anyway.
  Socket sock = *createSocket();

  do {
    Socket nsock;
    if (std::optional<Socket> p = sock.acceptConn()) {
      sock = *p;
    }

    auto msg = *sock.recvData(11);
    sock.sendData(msg);

  } while (true);
}
