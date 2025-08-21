#include "connection.hpp"
#include "utilities.hpp"
#include "tls/gtls_raii.hpp"
#include <unistd.h>
#include <iostream>
#include <span>
#include <signal.h>



auto main() -> int
{
  std::string version = gnutls_check_version(nullptr);
  printDebug(std::format("Using GnuTLS version: {}", version));

  CertStore store;
  store.loadCredentials("./ca-cert.pem", "./ca-key.pem", true);

  signal(SIGPIPE, SIG_IGN);
  // We want to crash here anyway.
  Socket sock = *createSocket();
  Session<CertStore> session(store, GNUTLS_SERVER | GNUTLS_NO_SIGNAL);
  do {
    Socket nsock = *sock.acceptConn();
    session.setTransport(nsock);
    session.handshake();
    printDebug("Handshake complete. Wohoo!");
    auto msg = *nsock.recvData(11);
    nsock.sendData<std::byte>(msg);
  } while (true);
}
