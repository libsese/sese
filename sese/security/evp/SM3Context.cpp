#include <sese/security/evp/SM3Context.h>

#include <openssl/evp.h>

sese::security::evp::SM3Context::SM3Context() noexcept {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit(static_cast<EVP_MD_CTX *>(this->context), EVP_sm3());
}

sese::security::evp::SM3Context::~SM3Context() noexcept {
    EVP_MD_CTX_free(static_cast<EVP_MD_CTX *>(this->context));
}

void sese::security::evp::SM3Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate(static_cast<EVP_MD_CTX *>(this->context), buffer, len);
}

void sese::security::evp::SM3Context::final() noexcept {
    EVP_DigestFinal(static_cast<EVP_MD_CTX *>(this->context), result, nullptr);
}

void *sese::security::evp::SM3Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::SM3Context::getLength() noexcept {
    return length;
}
