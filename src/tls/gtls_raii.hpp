#pragma once
#include <gnutls/gnutls.h>
#include <memory>
#include <variant>
#include <type_traits>
#include <functional>
#include <iostream>
#include <filesystem>
#include <expected>
#include "tls_errors.hpp"
#include "../utilities.hpp"
#include "../socket.hpp"

template<typename T>
concept Store = requires(T a)
{
  a.getCredentials();
  a.loadCredentials(std::declval<std::filesystem::path>(), std::declval<std::filesystem::path>(), bool{});
};

class CertStore {
public:
  CertStore();
  ~CertStore();
  CertStore(const CertStore&) = delete;
  CertStore& operator=(const CertStore&) = delete;
  CertStore(CertStore&&) = delete;
  CertStore& operator=(CertStore&&) = delete;

  void loadCredentials(const std::filesystem::path&, const std::filesystem::path&, bool);
  gnutls_certificate_credentials_t getCredentials();

private:
  gnutls_certificate_credentials_t cert;
};

template<Store T>
class Session {
public:
  Session(std::shared_ptr<T>& store, unsigned flags)
  {
    this->store = store;
    if (int i = gnutls_init(&this->session,flags); i != GNUTLS_E_SUCCESS) {
      const std::string err = gnutls_strerror(i);
      throw std::runtime_error(std::format("Could not initialize GnuTLS.", err));
    }

    if (int i = gnutls_set_default_priority(this->session); i != GNUTLS_E_SUCCESS) {
      const std::string err = gnutls_strerror(i);
      const std::string format_string = std::format("Setting priority failed: {}", err);
      print_debug(format_string);
    }

    // TODO: Remove GNUTLS_CRD_CERTIFICATE here and make it method-agnostic
    gnutls_certificate_credentials_t cert = store->getCredentials();

    if (int i = gnutls_credentials_set(this->session, GNUTLS_CRD_CERTIFICATE, cert); i != GNUTLS_E_SUCCESS) {
      const std::string err = gnutls_strerror(i);
      const std::string format_string = std::format("Setting certificate as default credential failed: {}", err);
      print_debug(format_string);
    }

    gnutls_handshake_set_timeout(this->session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
    gnutls_certificate_server_set_request(this->session, GNUTLS_CERT_IGNORE);
  };

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;
  Session& operator=(Session&& that)
  {
    if (this != &that) {
      gnutls_deinit(this->session);
      this->session = std::move(that.session);
    }
    return *this;
  }

  Session(Session&& that)
  {
    this->session = std::move(that.session);
  };

  std::expected<void, handshake_errors> handshake(int retries)
  {
    if (retries == 0) {
      return std::unexpected(handshake_errors::RETRIES_EXHAUSTED);
    }
    int i = gnutls_handshake(this->session);
    if (i < 0) {
      const std::string err = gnutls_strerror(i);
      const std::string format_string = std::format("Handshake failed: {}", err);
      print_debug(format_string);
      return std::unexpected(handshake_errors::FATAL_ALERT);
    }

    if (gnutls_error_is_fatal(i) == 0) {
      // retry.
      return Session::handshake(retries - 1);
    }

    return {};
  };


  void setTransport(Socket sock)
  {
    int fd = sock.getRawSock();
    gnutls_transport_set_int(this->session, fd);
  };

  ~Session()
  {
    gnutls_deinit(session);
  };

private:
  gnutls_session_t session;
  std::shared_ptr<T> store;
};
