#include <openssl/ssl.h>

#include <sese/security/SSLContext.h>

using namespace sese;

security::SSLContext::SSLContext(const void *method) noexcept {
    context = SSL_CTX_new((const SSL_METHOD *)method);
}

security::SSLContext::~SSLContext() noexcept {
    SSL_CTX_free((SSL_CTX *)context);
}

void *security::SSLContext::getContext() const noexcept {
    return context;
}

bool security::SSLContext::importCertFile(const char *file, int type) noexcept {
    return 1 == SSL_CTX_use_certificate_file((SSL_CTX *)context, file, type);
}

bool security::SSLContext::importPrivateKey(const char *file, int type) noexcept {
    return 1 == SSL_CTX_use_PrivateKey_file((SSL_CTX *)context, file, type);
}

bool security::SSLContext::authPrivateKey() noexcept {
    return 1 == SSL_CTX_check_private_key((SSL_CTX *)context);
}

Socket::Ptr security::SSLContext::newSocketPtr(Socket::Family family, int32_t flags) {
    return std::make_shared<SecuritySocket>(shared_from_this(), family, flags);
}