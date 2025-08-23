#include "gtls_raii.hpp"
#include "../utilities.hpp"
#include <string>
#include <format>

CertStore::CertStore() {
  gnutls_certificate_credentials_t cred;
  if (int i = gnutls_certificate_allocate_credentials(&cred); i != GNUTLS_E_SUCCESS) {
    throw std::runtime_error("Could not initialize Store.");
  };

  if (int i = gnutls_certificate_set_x509_system_trust(cred); i < 0) {
    throw std::runtime_error("Could not get trusted authorities from the system store.");
  }

  this->cert = std::move(cred);
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
