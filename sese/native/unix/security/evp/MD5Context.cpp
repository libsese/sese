#include <sese/security/evp/MD5Context.h>

#include <openssl/evp.h>

sese::security::evp::MD5Context::MD5Context() noexcept : Context() {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit((EVP_MD_CTX *) this->context, EVP_md5());
}

sese::security::evp::MD5Context::~MD5Context() noexcept {
    EVP_MD_CTX_free((EVP_MD_CTX *) this->context);
}

void sese::security::evp::MD5Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate((EVP_MD_CTX *) this->context, buffer, len);
}

void sese::security::evp::MD5Context::final() noexcept {
    EVP_DigestFinal((EVP_MD_CTX *) this->context, result, nullptr);
}

void *sese::security::evp::MD5Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::MD5Context::getLength() noexcept {
    return length;
}