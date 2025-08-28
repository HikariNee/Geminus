#include <gnutls/gnutls.h>
#include <signal.h>
#include <unistd.h>

import std;
import TLS;
import TLS_Errors;
import Utils;
import Socket;
import Connection;
import Gemini;

auto main() -> int
{
  const std::string version = std::format("GnuTLS Version: {}", TLS::getVersion());
  print_debug(version);

  std::shared_ptr<TLS::CertStore> store = std::make_shared<TLS::CertStore>();
  store->loadCredentials("./ca-cert.pem", "./ca-key.pem", true);

  signal(SIGPIPE, SIG_IGN);

  // We want to crash here anyway.
  Socket sock = *createSocket();

  do {
    TLS::Session<TLS::CertStore> session(store, GNUTLS_SERVER | GNUTLS_NO_SIGNAL);
    std::optional<std::pair<Socket, sockaddr>> nsock = sock.acceptConn();

    if (nsock.has_value()) {
      auto [newsock, addr] = nsock.value();
      session.setTransport(newsock);
      auto handshake = session.handshake(3);

      if (handshake.has_value()) {
        auto retbuf = session.recv(4).value();
        std::cout << buf_to_string(retbuf) << std::endl;

      } else {
        switch (handshake.error()) {
        case handshake_errors::RETRIES_EXHAUSTED:
          std::cout << "retries died." << '\n';
          break;
        case handshake_errors::FATAL_ALERT:
          std::cout << "Fatal alert died." << '\n';
          break;
        }
      }
    }
  } while (true);
}
