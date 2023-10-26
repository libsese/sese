#include <sese/security/evp/SHA256Context.h>

#include <openssl/evp.h>

sese::security::evp::SHA256Context::SHA256Context() noexcept : Context() {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit((EVP_MD_CTX *) this->context, EVP_sha256());
}

sese::security::evp::SHA256Context::~SHA256Context() noexcept {
    EVP_MD_CTX_free((EVP_MD_CTX *) this->context);
}

void sese::security::evp::SHA256Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate((EVP_MD_CTX *) this->context, buffer, len);
}

void sese::security::evp::SHA256Context::final() noexcept {
    EVP_DigestFinal((EVP_MD_CTX *) this->context, result, nullptr);
}

void *sese::security::evp::SHA256Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::SHA256Context::getLength() noexcept {
    return 32;
}