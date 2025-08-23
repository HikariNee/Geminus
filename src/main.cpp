#include "connection.hpp"
#include "tls/tls.hpp"
#include "utilities.hpp"
#include <iostream>
#include <signal.h>
#include <span>
#include <unistd.h>

auto main() -> int
{
  const std::string version = std::format("using GnuTLS Version: {}", gnutls_check_version(nullptr));
  print_debug(version);

  std::shared_ptr<CertStore> store = std::make_shared<CertStore>();
  store->loadCredentials("./ca-cert.pem", "./ca-key.pem", true);

  signal(SIGPIPE, SIG_IGN);
  // We want to crash here anyway.
  Socket sock = *createSocket();
  Session<CertStore> session(store, GNUTLS_SERVER | GNUTLS_NO_SIGNAL);

  do {
    Socket nsock = *sock.acceptConn();
    session.setTransport(nsock);
    if (auto p = session.handshake(3); !p.has_value()) {
      continue;
    }

  } while (true);
}
