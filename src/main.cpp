#include <signal.h>
#include <unistd.h>

import std;
import TLS;
import Utils;
import Socket;
import Connection;

auto main() -> int
{
  const std::string version = std::format("GnuTLS Version: {}", TLS::getVersion());
  print_debug(version);

  std::shared_ptr<TLS::CertStore> store = std::make_shared<TLS::CertStore>();
  store->loadCredentials("./ca-cert.pem", "./ca-key.pem", true);

  signal(SIGPIPE, SIG_IGN);

  // We want to crash here anyway.
  Socket sock = *createSocket();
  TLS::Session<TLS::CertStore> session(store, GNUTLS_SERVER | GNUTLS_NO_SIGNAL);

  do {
    std::optional<Socket> nsock = sock.acceptConn();
    if (nsock.has_value()) {
      Socket newsock = nsock.value();
      session.setTransport(newsock);
      if (auto p = session.handshake(3); !p.has_value()) {
        continue;
      }
    }
  } while (true);
}
