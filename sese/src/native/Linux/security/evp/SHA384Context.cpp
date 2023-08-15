#include <sese/security/evp/SHA384Context.h>

#include <openssl/evp.h>

sese::security::evp::SHA384Context::SHA384Context() noexcept : Context() {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit((EVP_MD_CTX *) this->context, EVP_sha384());
}

sese::security::evp::SHA384Context::~SHA384Context() noexcept {
    EVP_MD_CTX_free((EVP_MD_CTX *) this->context);
}

void sese::security::evp::SHA384Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate((EVP_MD_CTX *) this->context, buffer, len);
}

void sese::security::evp::SHA384Context::final() noexcept {
    EVP_DigestFinal((EVP_MD_CTX *) this->context, result, nullptr);
}

void *sese::security::evp::SHA384Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::SHA384Context::getLength() noexcept {
    return 48;
}