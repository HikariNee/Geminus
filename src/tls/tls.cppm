module;

#include <gnutls/gnutls.h>

export module TLS;
import std;
import TLS_Errors;
import Utils;
import Socket;

export using enum gnutls_init_flags_t;
export class CertStore;

template<typename T>
concept Store = requires(T a)
{
  a.getCredentials();
  a.loadCredentials(std::declval<std::filesystem::path>(), std::declval<std::filesystem::path>(), bool{});
};

export namespace TLS {
  auto getVersion() -> std::string
  {
    return std::string{gnutls_check_version(nullptr)};
  }

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

  CertStore::CertStore() {
    if (int i = gnutls_certificate_allocate_credentials(&this->cert); i != GNUTLS_E_SUCCESS) {
      throw std::runtime_error("Could not initialize Store.");
    };

    if (int i = gnutls_certificate_set_x509_system_trust(this->cert); i < 0) {
      throw std::runtime_error("Could not get trusted authorities from the system store.");
    }
  }

  CertStore::~CertStore() {
    gnutls_certificate_free_credentials(cert);
  };

  auto CertStore::loadCredentials(const std::filesystem::path& certfile, const std::filesystem::path& certkey, bool selfSigned) -> void
  {
    if (int i = gnutls_certificate_set_x509_key_file2(this->cert, certfile.c_str(), certkey.c_str(), GNUTLS_X509_FMT_PEM, nullptr, 0); i != GNUTLS_E_SUCCESS) {
      throw std::runtime_error("Could not load credentials, check the paths provided.");
    }

    if (selfSigned) {
      if (int i = gnutls_certificate_set_x509_trust_file(this->cert, certfile.c_str(), GNUTLS_X509_FMT_PEM); i < 0) {
	throw std::runtime_error("Could not set self signed cert as trusted.");
      }
    }
    print_debug("Loaded credentials");
  }

  auto CertStore::getCredentials() -> gnutls_certificate_credentials_t
  {
    return cert;
  }

  template<Store T>
  class Session {
  public:
    Session(const std::shared_ptr<T>& store, unsigned flags);
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session& operator=(Session&&) = delete;
    Session(Session&&) = delete;
    std::expected<void, handshake_errors> handshake(int retries);
    void setTransport(Socket sock);
    ~Session();

  private:
    gnutls_session_t session;
    std::shared_ptr<T> store;
  };

  template<Store T>
  Session<T>::Session(const std::shared_ptr<T>& store, unsigned flags)
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

  template<Store T>
  auto Session<T>::handshake(int retries) -> std::expected<void, handshake_errors>
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

  template<Store T>
  auto Session<T>::setTransport(Socket sock) -> void
  {
    int fd = sock.getRawSock();
    gnutls_transport_set_int(this->session, fd);
  };

  template<Store T>
  Session<T>::~Session()
  {
    gnutls_deinit(session);
  };

}
