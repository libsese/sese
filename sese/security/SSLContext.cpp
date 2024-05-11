#include <openssl/ssl.h>

#include <sese/security/SSLContext.h>
#include <sese/record/Marco.h>

#include <cassert>

using namespace sese::security;

SSLContext::SSLContext(const void *method) noexcept {
    context = SSL_CTX_new(static_cast<const SSL_METHOD *>(method));
}

SSLContext::~SSLContext() noexcept {
    if (context) {
        SSL_CTX_free(static_cast<SSL_CTX *>(context));
    }
}

void *SSLContext::release() noexcept {
    assert(context);
    auto rt = context;
    context = nullptr;
    return rt;
}

void *SSLContext::getContext() const noexcept {
    assert(context);
    return context;
}

bool SSLContext::importCertFile(const char *file) noexcept {
    assert(context);
    return 1 == SSL_CTX_use_certificate_file(static_cast<SSL_CTX *>(context), file, SSL_FILETYPE_PEM);
}

bool SSLContext::importPrivateKeyFile(const char *file) noexcept {
    assert(context);
    return 1 == SSL_CTX_use_PrivateKey_file(static_cast<SSL_CTX *>(context), file, SSL_FILETYPE_PEM);
}

bool SSLContext::authPrivateKey() noexcept {
    assert(context);
    return 1 == SSL_CTX_check_private_key(static_cast<SSL_CTX *>(context));
}

// bool SSLContext::verifyAndLoad(const char *file) noexcept {
//     SSL_CTX_set_verify((SSL_CTX *) context, SSL_VERIFY_PEER, nullptr);
//     return 1 == SSL_CTX_load_verify_locations((SSL_CTX *) context, file, nullptr);
// }

sese::net::Socket::Ptr SSLContext::newSocketPtr(Socket::Family family, int32_t flags) {
    assert(context);
    return std::make_shared<SecuritySocket>(shared_from_this(), family, flags);
}