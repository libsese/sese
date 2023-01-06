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

bool security::SSLContext::importCertFile(const char *file, FileType type) noexcept {
    int t = SSL_FILETYPE_PEM;
    if (type == ASN1) t = SSL_FILETYPE_ASN1;
    return 1 == SSL_CTX_use_certificate_file((SSL_CTX *)context, file, t);
}

bool security::SSLContext::importPrivateKey(const char *file, FileType type) noexcept {
    int t = SSL_FILETYPE_PEM;
    if (type == ASN1) t = SSL_FILETYPE_ASN1;
    return 1 == SSL_CTX_use_PrivateKey_file((SSL_CTX *)context, file, t);
}

bool security::SSLContext::authPrivateKey() noexcept {
    return 1 == SSL_CTX_check_private_key((SSL_CTX *)context);
}

bool security::SSLContext::verifyAndLoad(const char *file) noexcept {
    SSL_CTX_set_verify((SSL_CTX *) context, SSL_VERIFY_PEER, nullptr);
    return 1 == SSL_CTX_load_verify_locations((SSL_CTX *) context, file, nullptr);
}

Socket::Ptr security::SSLContext::newSocketPtr(Socket::Family family, int32_t flags) {
    return std::make_shared<SecuritySocket>(shared_from_this(), family, flags);
}