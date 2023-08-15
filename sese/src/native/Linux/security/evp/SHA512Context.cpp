#include <sese/security/evp/SHA512Context.h>

#include <openssl/evp.h>

sese::security::evp::SHA512Context::SHA512Context() noexcept : Context() {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit((EVP_MD_CTX *) this->context, EVP_sha512());
}

sese::security::evp::SHA512Context::~SHA512Context() noexcept {
    EVP_MD_CTX_free((EVP_MD_CTX *) this->context);
}

void sese::security::evp::SHA512Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate((EVP_MD_CTX *) this->context, buffer, len);
}

void sese::security::evp::SHA512Context::final() noexcept {
    EVP_DigestFinal((EVP_MD_CTX *) this->context, result, nullptr);
}

void *sese::security::evp::SHA512Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::SHA512Context::getLength() noexcept {
    return 64;
}